/**
 * The MIT License (MIT)
 *  
 * Copyright (c) Taketoshi Aono(brn)
 *  
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *  
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *  
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**
 * @fileoverview Entry point.
 * @author Taketoshi Aono
 */


var exec = require('child_process').exec;
var os = require('os');
var platform = os.platform();
var test;
var argv = process.argv[2];
var fs = require('fs');
var STAMP = '.stamp';
if (platform === 'darwin') {
  test = 'sh ./all-test.osx.sh';
} else if (platform === 'win32') {
  test = './all-test.win.bat';
}

function proc(cmd, cb) {
  var p = exec(cmd, {encoding: 'utf8'});
  ['stderr', 'stdout'].forEach(function(o) {
    p[o].pipe(process[o]);
  });
  p.on('error', function(e) {
    throw e;
  });

  if (cb) {
    p.on('exit', cb);
  }
}


function run() {
  if (argv === 'test') {
    proc('python run_gyp.py', function() {
      proc(test);
    });
  } else {
    proc('node-gyp rebuild');
  }
}



if (fs.existsSync(STAMP)) {
  run();
} else {
  proc('python configure.py', function() {
    fs.writeFileSync(STAMP, new Date().toString(), 'utf8');
    run();
  });
}
