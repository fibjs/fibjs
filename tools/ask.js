const db = require('db');
const util = require('util');
const ChatGTP = require('./util/chatgpt');

const chatgpt = new ChatGTP(process.env.OPENAI_API_KEY);

const conn = db.open('sqlite:./temp/docs.db');
const prompt = `You are a fibjs development assistant, please answer the questions and explain in detail strictly based on the following information, and be sure not to refer to the nodejs documentation and source code`;
const modules = `fibjs has the following modules built in: ${util.buildInfo().modules.join(',')}`;

while (true) {
    var question = console.readLine("Ask a question: ");
    var ask_embedding = chatgpt.get_embedding(question);

    var contents = conn.execute(`SELECT docs.id, docs.text, docs.total_tokens, distance FROM doc_index, docs WHERE vec_search(doc_index.vec, "${JSON.stringify(ask_embedding.data[0].embedding)}:50") AND docs.rowid = doc_index.rowid ORDER BY distance`);
    console.log('top distance:', contents[0].distance);

    var content_tokens = 0;
    for (var i = 0; i < contents.length; i++) {
        if (content_tokens < 2000)
            content_tokens += contents[i].total_tokens;
        else
            break;
    }

    contents = contents.slice(0, i);

    var messages = [
        {
            role: 'system',
            content: prompt
        },
        {
            role: 'system',
            content: modules
        }
    ];

    contents.forEach((content) => {
        // console.notice(`id: ${content.id} distance: ${content.distance}`);
        // console.log('content:', content.text);
        messages.push({
            role: 'system',
            content: content.text
        });
    });

    messages.push({
        role: 'user',
        content: question
    });

    var answer = chatgpt.chat(messages);

    console.warn("=================================================================")
    console.log('answer:', answer);
}
