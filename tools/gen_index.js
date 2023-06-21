const fs = require('fs');
const path = require('path');
const db = require('db');
const hash = require('hash');
const { split_folder } = require('fib-spliter');
const ChatGTP = require('./util/chatgpt');

const chatgpt = new ChatGTP(process.env.OPENAI_API_KEY);

var nodes = split_folder([
    path.join(__dirname, '../docs/web/dist/docs/guide'),
    path.join(__dirname, '../docs/web/dist/docs/manual/module/ifs'),
    path.join(__dirname, '../docs/web/dist/docs/manual/object/ifs'),
    path.join(__dirname, '../test')
], {
    ".html": {
        ignore_tags: [
            'svg'
        ],
        before_split: function (html) {
            html = html.replace(/<p>[0-9\r\n]*<\/p>/g, '');
            html = html.replace(/<h2 /g, '</div><div><h2 ');
            html = html.replace(/<h3 /g, '</div><div><h3 ');
            html = html.replace(/<code>/g, '<code>```\n');
            html = html.replace(/<\/code>/g, '\n```</code>');
            return html;
        }
    }
});

var infos = fs.readdir(path.join(__dirname, '../docs/infos'));
infos.forEach(function (info) {
    var fname = path.join(__dirname, '../docs/infos', info);
    console.log(`loading ${fname}`);
    nodes.push(fs.readTextFile(fname));
});

console.log(nodes.length, 'nodes');

var db_path = __dirname + '/temp';
if (!fs.stat(db_path)) {
    fs.mkdir(db_path);
}

var conn = db.open(`sqlite:${db_path}/docs.db`);

conn.execute('CREATE TABLE IF NOT EXISTS fibjs_docs (id PRIMARY KEY, deleted DEFAULT 0, text, embedding, total_tokens)');
conn.execute("CREATE VIRTUAL TABLE IF NOT EXISTS fibjs_index USING vec_index(vec(1536))");

var res = conn.execute('SELECT id, deleted FROM fibjs_docs');
var docs = {};
res.forEach(element => {
    docs[element.id] = element.deleted;
});

var ids = new Set();
var updated = false;

nodes.forEach(function (node) {
    if (node.length > 32) {
        var id = hash.md5(node).digest('hex');

        if (!ids.has(id)) {
            ids.add(id);

            if (docs[id] === undefined) {
                console.log('inserting', id);
                var res = chatgpt.get_embedding(node);
                updated = true;
                conn.execute('INSERT INTO fibjs_docs (id, text, embedding, total_tokens) VALUES (?, ?, ?, ?)', id, node,
                    JSON.stringify(res.data[0].embedding), res.usage.total_tokens);
            } else if (docs[id] === 1) {
                console.log('updating', id);
                updated = true;
                conn.execute('UPDATE fibjs_docs SET deleted = 0 WHERE id = ?', id);
            }

            delete docs[id];
        }
    }
});

for (var id in docs) {
    if (docs[id] === 0) {
        console.log('deleting', id);
        updated = true;
        conn.execute('UPDATE fibjs_docs SET deleted = 1 WHERE id = ?', id);
    }
}

if (updated) {
    console.log("rebuild index...");
    conn.execute("DELETE FROM fibjs_index");
    conn.execute(`INSERT INTO fibjs_index(rowid, vec) SELECT rowid, embedding FROM fibjs_docs WHERE deleted = 0`);
}

conn.close();

console.log("done");
