const coap = require('../') // or coap
const req = coap.request('coap://localhost:5683/co2')

req.on('response', (res) => {
    res.pipe(process.stdout)
})

req.end()
