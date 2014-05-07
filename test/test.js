var assert = require('assert');
var nis = require('../build/Release/nis');
assert.equal('object', typeof(nis));

describe('NIS', function() {
    describe('create object', function() {
        it('should return object with default domain', function() {
            var yp = nis();
            assert.equal('object', typeof(yp));
        })
    })
});
