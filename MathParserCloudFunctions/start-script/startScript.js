const { Storage } = require('@google-cloud/storage');

exports.startScript = async (req, res) => {

    res.set('Access-Control-Allow-Origin', '*');

    if (req.method === 'OPTIONS') {
        res.set('Access-Control-Allow-Methods', 'GET, POST');
        res.set('Access-Control-Allow-Headers', 'Content-Type');
        res.set('Access-Control-Max-Age', '86400');
        res.status(204).send('');
    } else {

        const storage = new Storage();

        let session = await getSession(storage, req.query.sessionId);

        let file = storage
            .bucket("mathparser-userdata")
            .file("startscript-" + session.user.name + "-" + session.user.id);

        if (req.method === 'GET') {

            let readStream = file.createReadStream();
            readStream.on('data', (data) => {
                res.write(data);
            });
            readStream.on('end', (data) => {
                res.status(200).send();
            });

        } else if (req.method === 'POST') {

            file.save(req.body);
            let jsonRes = {};
            jsonRes.result = "Gelukt!" + JSON.stringify(session);
            res.json(jsonRes);
        }

    }
}

//await-able because returns a Promise
function streamToString(stream) {
    const chunks = [];
    return new Promise((resolve, reject) => {
        stream.on('data', (chunk) => chunks.push(Buffer.from(chunk)));
        stream.on('error', (err) => reject(err));
        stream.on('end', () => resolve(Buffer.concat(chunks).toString('utf8')));
    })
}

async function getSession(storage, sessionId) {
    let readStream = await storage
        .bucket("mathparser-session-data")
        .file(sessionId)
        .createReadStream();

    let strSession = await streamToString(readStream);

    return JSON.parse(strSession);
}