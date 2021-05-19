using MathParserLib;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;

namespace MathParserWPF
    {
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
        {
        public string Output
            {
            get { return (string)GetValue(OutputProperty); }
            set { SetValue(OutputProperty, value); }
            }

        // Using a DependencyProperty as the backing store for Output.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty OutputProperty =
            DependencyProperty.Register("Output", typeof(string), typeof(MainWindow), new PropertyMetadata(""));


        public string Formula
            {
            get { return (string)GetValue(FormulaProperty); }
            set { SetValue(FormulaProperty, value); }
            }

        // Using a DependencyProperty as the backing store for FormmulaProp.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty FormulaProperty =
            DependencyProperty.Register("Formula", typeof(string), typeof(MainWindow), new PropertyMetadata("", new PropertyChangedCallback(OnFormulaChanged)));

        private static void OnFormulaChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
            {
            MainWindow wnd = d as MainWindow;
            wnd.Output = "";
            ParseFormula(wnd.Formula, wnd);
            }

        private static void ParseFormula(string formula, MainWindow wnd)
            {
            parseWithCSharp(formula, wnd);
            parseWithDll(formula, wnd);
            }

        private static void parseWithDll(string formula, MainWindow wnd)
            {
            DllInterface.Parse(formula);
            var strJson = DllInterface.GetResult();
            //wnd.Output += strJson;
            var jsonResult = JsonSerializer.Deserialize<JsonResults>(strJson);
            wnd.Output += "\n";
            foreach (var result in jsonResult.result)
                {
                double number;
                bool isNum = double.TryParse(result.value, out number);
                wnd.Output += (result.id == "#result#" ? "" : result.id + "=") + (isNum ? number.ToString("0.#######") : result.value) + result.unit;
                if (result.errors.Length > 0)
                    {
                    wnd.Output += " <<< ";
                    foreach (var err in result.errors)
                        wnd.Output += err.message + "  ";
                    }
                wnd.Output += "\n";
                }
            }

        private static void parseWithCSharp(string formula, MainWindow wnd)
            {
            //Tokenizer tok = new Tokenizer(formula);
            Parser parser = new Parser(formula);
            var statements = parser.parse();
            Resolver resolver = new Resolver(parser);
            statements.ForEach(stmt =>
            {
                var result = resolver.resolve(stmt);
                if (result.id != null)
                    wnd.Output += result.id.StringValue + "=";
                if (result.number == Double.NaN)
                    wnd.Output = "NaN";
                else
                    wnd.Output += result.ToString("0.#######");
                wnd.Output += "\n";
            });
            }

        public MainWindow()
            {
            InitializeComponent();
            Formula = "";
            this.DataContext = this;
            txtFormula.Focus();
            }
        }
    }
