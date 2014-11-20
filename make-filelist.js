/**
 * @fileoverview
 * @author Taketoshi Aono
 */

"use strict";

var glob = require('glob');
var fs = require('fs');

fs.writeFileSync("test/parser/parser-test-case/filelist", glob.sync("test/microsoft/typescript/tests/cases/**/*.ts").join("\n"), "utf8");
