const http = require('http');

const hc = new http.Client();
if (process.env.http_proxy || process.env.HTTP_PROXY)
    hc.http_proxy = process.env.http_proxy || process.env.HTTP_PROXY;

if (process.env.https_proxy || process.env.HTTPS_PROXY)
    hc.https_proxy = process.env.https_proxy || process.env.HTTPS_PROXY;

class ChatGTP {
    constructor(api_key) {
        this._api_key = api_key;
    }

    get_embedding(text) {
        var res = hc.post('https://api.openai.com/v1/embeddings', {
            headers: {
                'Authorization': `Bearer ${this._api_key}`
            },
            json: {
                "input": text,
                "model": "text-embedding-ada-002"
            }
        });

        if (res.statusCode > 300)
            throw new Error(res.json().error.message);

        return res.json();
    }

    chat(msg) {
        var res = hc.post('https://api.openai.com/v1/chat/completions', {
            headers: {
                'Authorization': `Bearer ${this._api_key}`
            },
            json: {
                "model": "gpt-3.5-turbo",
                "messages": msg
            }
        });

        if (res.statusCode > 300)
            throw new Error(res.json().error.message);

        return res.json().choices[0].message.content;
    }
}

module.exports = ChatGTP;
