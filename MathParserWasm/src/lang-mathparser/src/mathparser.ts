import {parser} from "./parser.js" 
import {LRLanguage, LanguageSupport, foldNodeProp, foldInside} from "@codemirror/language"
import {styleTags, tags as t} from "@codemirror/highlight"

export const mathparserLanguage = LRLanguage.define({
  parser: parser.configure({
    props: [
      foldNodeProp.add({
        "Block": foldInside,
        BlockComment(tree) { return {from: tree.from + 2, to: tree.to - 2} }
      }),
      styleTags({
        Super: t.atom,
        BooleanLiteral: t.bool,
        Star: t.modifier,
        VariableName: t.variableName,
        "CallExpression/VariableName": t.function(t.variableName),
        VariableDefinition: t.definition(t.variableName),
        Label: t.labelName,
        PropertyName: t.propertyName,
        UpdateOp: t.updateOperator,
        LineComment: t.lineComment,
        BlockComment: t.blockComment,
        Number: t.number,
        String: t.string,
        ArithOp: t.arithmeticOperator,
        LogicOp: t.logicOperator,
        BitOp: t.bitwiseOperator,
        CompareOp: t.compareOperator,
        Equals: t.definitionOperator,
        "( )": t.paren,
        "[ ]": t.squareBracket,
        "{ }": t.brace,
        ".": t.derefOperator,
        ", ;": t.separator,
        Annot: t.processingInstruction
      })
    ]
  }),
  languageData: {
    closeBrackets: {brackets: ["(", "[", "{", "'", '"', "`"]},
    commentTokens: {line: "//", block: {open: "/*", close: "*/"}},
    indentOnInput: /^\s*(?:case |default:|\{|\}|<\/)$/,
    wordChars: "$"
  }
})

export function mathparser() {
  let lang = mathparserLanguage
  return new LanguageSupport(lang, mathparserLanguage.data.of({}))
}
