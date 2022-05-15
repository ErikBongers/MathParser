const { Storage } = require('@google-cloud/storage');

exports.uploadStartScript = async (req, res) => {


    res.set('Access-Control-Allow-Origin', '*');

    if (req.method === 'OPTIONS') {
        res.set('Access-Control-Allow-Methods', 'GET, POST');
        res.set('Access-Control-Allow-Headers', 'Content-Type');
        res.set('Access-Control-Max-Age', '86400');
        res.status(204).send('');
    } else {

        let sessionId = req.query.sessionId;

        const storage = new Storage();

        let readStream = await storage
            .bucket("mathparser-session-data")
            .file(sessionId)
            .createReadStream();

        function streamToString(stream) {
            const chunks = [];
            return new Promise((resolve, reject) => {
                stream.on('data', (chunk) => chunks.push(Buffer.from(chunk)));
                stream.on('error', (err) => reject(err));
                stream.on('end', () => resolve(Buffer.concat(chunks).toString('utf8')));
            })
        }

        let strSession = await streamToString(readStream);

        let session = JSON.parse(strSession);

        await storage
            .bucket("mathparser-userdata")
            .file("startscript-"+session.name + "-" + session.id)
            .save(req.body);

        let jsonRes = {};
        jsonRes.result = "Gelukt!" + JSON.stringify(session);
        res.json(jsonRes);

    }

  };