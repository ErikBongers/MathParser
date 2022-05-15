const { Storage } = require('@google-cloud/storage');
const { OAuth2Client } = require('google-auth-library');
const CLIENT_ID = "595809100273-9vvaauhe57ovk2s67g6buuqptsiamfrg.apps.googleusercontent.com";
const client = new OAuth2Client(CLIENT_ID);
const uuid = require('uuid');

async function verify(token) {
    const ticket = await client.verifyIdToken({
        idToken: token,
        audience: CLIENT_ID, // or [CLIENT_ID_1, CLIENT_ID_2, ...]
    });
    const payload = ticket.getPayload();
    let user = {};
    user.id = payload['sub'];
    user.name = payload['name'];
    return user;
}

exports.getSession = async (req, res) => {
    // Set CORS headers for preflight requests
    // Allows GETs from any origin with the Content-Type header
    // and caches preflight response for 3600s

    res.set('Access-Control-Allow-Origin', '*');

    if (req.method === 'OPTIONS') {
        res.set('Access-Control-Allow-Methods', 'GET, POST');
        res.set('Access-Control-Allow-Headers', 'Content-Type');
        res.set('Access-Control-Max-Age', '86400');
        res.status(204).send('');
    } else {
        let session = await verify(req.body.credential);
        session.sessionId = uuid.v1();

        let expirationTime = 2 * 60 * 60 * 1000;

        session.expirationDate = new Date();
        session.expirationDate.setTime(session.expirationDate.getTime() + expirationTime);
        //create session file.
        const storage = new Storage();
        await storage
            .bucket("mathparser-session-data")
            .file(session.sessionId)
            .save(JSON.stringify(session));


        res.status(200).json(session);
    }


};


