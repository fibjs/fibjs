const db = require('db');
const util = require('util');
const ChatGTP = require('./util/chatgpt');

const chatgpt = new ChatGTP(process.env.OPENAI_API_KEY);

const conn = db.open('sqlite:./temp/docs.db');
const prompt = `You are a fibjs development assistant, please answer the question strictly based on the following information in the most detail , do not refer to the nodejs documentation and source code`;
const modules = `fibjs has the following modules built in: ${util.buildInfo().modules.join(',')}`;

while (true) {
    var question = console.readLine("Ask a question: ");
    var ask_embedding = chatgpt.get_embedding(question);

    var content = [];
    var content_tokens = 0;

    var res = conn.execute(`SELECT docs.text, docs.total_tokens, distance FROM doc_index, docs WHERE vec_search(doc_index.vec, "${JSON.stringify(ask_embedding.data[0].embedding)}:50") AND docs.rowid = doc_index.rowid ORDER BY distance`);
    console.log('top distance:', res[0].distance);
    for (var i = 0; i < res.length; i++) {
        if (content_tokens < 2000) {
            content.push(res[i].text);
            content_tokens += res[i].total_tokens;
        }
        else
            break;
    }

    var answer = chatgpt.chat([
        {
            role: 'system',
            content: prompt
        },
        {
            role: 'assistant',
            content: modules
        },
        {
            role: 'assistant',
            content: content.join('\n')
        },
        {
            role: 'user',
            content: question
        }
    ]);

    console.log('answer:', answer);
}
