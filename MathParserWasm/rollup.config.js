import {nodeResolve} from "@rollup/plugin-node-resolve"
import typescript from '@rollup/plugin-typescript';

export default {
  input:  "./src/lang-mathparser/src/editor.js",
  output: {
    file: "./www/editor.bundle.js",
    format: "iife",
    name: "cm"
  },
  plugins: [nodeResolve(), typescript()]
}