using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace MathParserLib
    {
    public abstract class Function
        {
        protected List<Value> args = new List<Value>();
        public abstract string getName();
        public void clearArgs() { args.Clear(); }
        public void addArg(Value arg) => args.Add(arg);
        public abstract Value execute();

        private static Dictionary<string, Function> functions;

        static void AddFunction(Function f)
            {
            functions.Add(f.getName(), f);
            }
        static Function()
            {
            functions = new Dictionary<string, Function>();
            AddFunction(new Sin());
            AddFunction(new Cos());
            AddFunction(new Tan());
            AddFunction(new ArcSin());
            AddFunction(new ArcCos());
            AddFunction(new ArcTan());
            AddFunction(new Asin());
            AddFunction(new Acos());
            AddFunction(new Atan());
            AddFunction(new Sqrt());
            }

        public static Function get(string functionName)
            {
            if (functions.ContainsKey(functionName))
                return functions[functionName];
            else
                return null;
            }
        public static bool exists(string functionName) => functions.ContainsKey(functionName);
        }


    public class Sin : Function
        {
        public override string getName() => "sin";
        public override Value execute()
            {
            if (args.Count != 1)
                return Double.NaN;
            double arg = args[0].number;
            if (args[0].unit != null && args[0].unit.Type == TokenType.DEG)
                arg = arg * Math.PI / 180;
            return Math.Sin(arg);
            }
        }

    public class Cos : Function
        {
        public override string getName() => "cos";
        public override Value execute()
            {
            if (args.Count != 1)
                return Double.NaN;
            double arg = args[0].number;
            if (args[0].unit != null && args[0].unit.Type == TokenType.DEG)
                arg = arg * Math.PI / 180;
            return Math.Cos(arg);
            }
        }

    public class Tan : Function
        {
        public override string getName() => "tan";
        public override Value execute()
            {
            if (args.Count != 1)
                return Double.NaN;
            double arg = args[0].number;
            if (args[0].unit != null && args[0].unit.Type == TokenType.DEG)
                arg = arg * Math.PI / 180;
            return Math.Tan(arg);
            }
        }

    public class ArcTan : Function
        {
        public override string getName() => "arctan";
        public override Value execute()
            {
            if (args.Count != 1)
                return Double.NaN;

            return Math.Atan(args[0].number);
            }
        }
    public class Atan : ArcTan { public override string getName() => "atan"; }

    public class ArcSin : Function
        {
        public override string getName() => "arcsin";
        public override Value execute()
            {
            if (args.Count != 1)
                return Double.NaN;

            return new Value(Math.Asin(args[0].number), new Token(TokenType.RAD));
            }
        }
    public class Asin : ArcSin { public override string getName() => "asin"; }

    public class ArcCos : Function
        {
        public override string getName() => "arccos";
        public override Value execute()
            {
            if (args.Count != 1)
                return Double.NaN;

            return new Value(Math.Acos(args[0].number), new Token(TokenType.RAD));
            }
        }
    public class Acos : ArcCos { public override string getName() => "acos"; }

    public class Sqrt : Function
        {
        public override string getName() => "sqrt";
        public override Value execute()
            {
            if (args.Count != 1)
                return Double.NaN;

            return Math.Sqrt(args[0].number);
            }
        }
    }
