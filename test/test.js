var assert = require('assert');

describe('NIS', function() {
    describe('require', function() {
        it('should return object', function() {
            var nis = require('../build/Release/nis');
            assert.equal('object', typeof(nis));
        })
    })
});
