
var path = require('path');
var fs = require('fs');

function requireBinding() {
    var v8 = 'v8-' + /[0-9]+\.[0-9]+/.exec(process.versions.v8)[0];
    var candidates = [
        [__dirname, 'build', 'Release', 'obj.target', 'nis.node'],
        [__dirname, 'bin', process.platform + '-' + process.arch + '-' + v8, 'nis.node']
    ];
    var candidate;

    for (var i = 0, l = candidates.length; i < l; i++) {
        candidate = path.join.apply(path.join, candidates[i]);

        if (fs.existsSync(candidate)) {
            return require(candidate);
        }
    }

    throw new Error('`nis` bindings not found');
}

module.exports = requireBinding();
