const path = require('path');
const db = require('db');
const hash = require('hash');
const split_folder = require('./util/split_folder');
const ChatGTP = require('./util/chatgpt');

const chatgpt = new ChatGTP(process.env.OPENAI_API_KEY);

var nodes = split_folder([
    path.join(__dirname, '../docs/web/dist/docs/guide'),
    path.join(__dirname, '../docs/web/dist/docs/manual/module/ifs'),
    path.join(__dirname, '../docs/web/dist/docs/manual/object/ifs'),
    path.join(__dirname, '../test')
]);

console.log(nodes.length, 'nodes');

const conn = db.open('sqlite:./temp/docs.db');

conn.execute('CREATE TABLE IF NOT EXISTS docs (id PRIMARY KEY, deleted DEFAULT 0, text, embedding, total_tokens)');
conn.execute("CREATE VIRTUAL TABLE IF NOT EXISTS doc_index USING vec_index(vec(1536))");

var res = conn.execute('SELECT id, deleted FROM docs');
var docs = {};
res.forEach(element => {
    docs[element.id] = element.deleted;
});

var ids = new Set();

nodes.forEach(function (node) {
    var id = hash.md5(node).digest('hex');

    if (!ids.has(id)) {
        ids.add(id);

        if (docs[id] === undefined) {
            console.log('inserting', id);
            var res = chatgpt.get_embedding(node);
            conn.execute('INSERT INTO docs (id, text, embedding, total_tokens) VALUES (?, ?, ?, ?)', id, node,
                JSON.stringify(res.data[0].embedding), res.usage.total_tokens);
        } else if (docs[id] === 1) {
            console.log('updating', id);
            conn.execute('UPDATE docs SET deleted = 0 WHERE id = ?', id);
        }

        delete docs[id];
    }
});

for (var id in docs) {
    if (docs[id] === 0) {
        console.log('deleting', id);
        conn.execute('UPDATE docs SET deleted = 1 WHERE id = ?', id);
    }
}

console.log("rebuild index...");
conn.execute("DELETE FROM doc_index");
conn.execute(`INSERT INTO doc_index(rowid, vec) SELECT rowid, embedding FROM docs WHERE deleted = 0`);

conn.close();

console.log("done");
