var assert = require('assert');
var nis = require('../build/Release/nis');

describe('NIS', function() {
    describe('create object', function() {
        it('should return object with default domain', function() {
            var yp = nis();
            assert.equal('object', typeof(yp));
            assert.equal('+kkcorp', yp.domain_name);
        })
    })
});
