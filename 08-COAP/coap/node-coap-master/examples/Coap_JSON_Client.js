const coap = require('../') // or coap
const bl = require('bl')

    coap.request({
            pathname: '/aqi',
            options: {
                Accept: 'application/json'
            }
        })
        .on('response', (res) => {
            console.log('response code', res.code)
            if (res.code !== '2.05') {
                return process.exit(1)
            }

            res.pipe(bl((err, data) => {
                if (err != null) {
                    process.exit(1)
                } else {
                    const json = JSON.parse(data)
                    console.log(json)
                    process.exit(0)
                }
            }))
        })
        .end();
