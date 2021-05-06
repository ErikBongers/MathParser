using Microsoft.VisualStudio.TestTools.UnitTesting;
using MathParserLib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MathParserLib.Tests
    {
    [TestClass()]
    public class ResolverTests
        {
        List<(string expr, string actual, string expected)> results = new List<(string, string, string)>();
        [TestMethod()]
        public void resolveTest()
            {
            testExpr("a", "0", ErrorId.VAR_NOT_DEF);
            //simple expressions, operator precedence
            testExpr("1+2", "3");
            testExpr("1-2", "-1");
            testExpr("(1+2)", "3");
            testExpr("(1DEG+2)", "3DEG");
            testExpr("3-4+5", "4");
            testExpr("3-(4+5)", "-6");
            testExpr("-3", "-3");

            //variables
            testExpr("a=1;b=2;a+b", "3");
            testExpr("a=-1;b=-2;a+b", "-3");
            //functions
            testExpr("sin(30DEG)", "0.5");
            //implicit multiplication
            testExpr("(1+2)3", "9");
            testExpr("3(1+2)", "9");
            testExpr("a=1;b=2;2a+b", "4");
            testExpr("a=1;b=2;2a+2b", "6");
            testExpr("a=1;b=2;c=3;(a+b)c", "9");
            testExpr("a=1;b=2;c=3;c(a+b)", "9");
            testExpr("a=3;b=4;c=5; a b c", "60");
            //implicit multiplication with warning
            testExpr("a=3;b=4; a/2b", "0.375", ErrorId.W_DIV_IMPL_MULT);
            testExpr("a=3;2a^2", "36", ErrorId.W_POW_IMPL_MULT);
            testExpr("a=3;2^2a", "64", ErrorId.W_POW_IMPL_MULT);

            if (results.Count > 0)
                {
                string output = "";
                results.ForEach(result => output += "\n" + result.expr + "  !=  " + result.actual + " (" + result.expected + ")");
                Assert.Fail(output);
                }
            }

        public void testExpr(string expr, string expectedResult, ErrorId errorId = ErrorId.NONE)
            {
            Parser parser = new Parser(expr);
            var statements = parser.parse();
            Resolver resolver = new Resolver(parser);
            Value result = new Value(-1);
            statements.ForEach(stmt =>
            {
                result = resolver.resolve(stmt);
            });
            if (expectedResult != result.ToString())
                {
                results.Add((expr, result.ToString(), expectedResult));
                }
            if (errorId == ErrorId.NONE)
                {
                if (result.errors.Count > 0)
                    results.Add((expr, result.ToString(), "unexpected error: " + result.errors[0].errorMsg));
                }
            else
                {
                var err = result.errors.FirstOrDefault(error => error.id == errorId);
                if (err == null)
                    results.Add((expr, result.ToString() + "+"+errorId.ToString(), "expected error: " + ErrorDefs.getMsg(errorId)));
                }
            }
        }
    }