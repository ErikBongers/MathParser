//using MathParserLib;
using System;
using System.Collections.Generic;
using System.IO;
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
            File.WriteAllText(localStorageFileName, wnd.Formula);
            ParseFormula(wnd.Formula, wnd);
            }

        private static void ParseFormula(string formula, MainWindow wnd)
            {
            //parseWithCSharp(formula, wnd);
            //wnd.Output += "\n";
            parseWithDll(formula, wnd);
            }

        private static void parseWithDll(string formula, MainWindow wnd)
            {
            DllInterface.Parse(formula);
            var strJson = DllInterface.GetResult();
            //wnd.Output += strJson;
            var jsonResult = JsonSerializer.Deserialize<JsonResults>(strJson);
            foreach (var result in jsonResult.result)
                {
                if (result.onlyComment)
                    {
                    wnd.Output += "//" + result.comment + "\n";
                    continue;
                    }
                string strComment = "";
                if (result.comment.Length != 0)
                    strComment = "//" + result.comment;

                string strResult = "";
                if (!result.mute || result.errors.Length > 0)
                    {
                    string resultVal = "";
                    if (result.type == "NUMBER")
                        {
                        double number;
                        bool isNum = double.TryParse(result.number.val, out number);
                        if (isNum)
                            {
                            if (result.number.fmt == "BIN" || result.number.fmt == "HEX")
                                resultVal = result.number.fmtd;
                            else
                                {
                                if (result.number.exp != 0)
                                    {
                                    double.TryParse(result.number.sig, out number);
                                    resultVal = number.ToString("0.#######") + "E" + result.number.exp;
                                    }
                                else
                                    resultVal = number.ToString("0.#######");
                                }
                            }
                        else
                            resultVal = result.number.val;
                        resultVal += result.number.u;
                        }
                    else if(result.type == "TIMEPOINT")
                        {
                        resultVal = result.date.formatted;
                        }
                    else if(result.type == "DURATION")
                        {
                        resultVal = result.duration.years + " years, " + result.duration.months + " months, " + result.duration.days + " days";
                        }
                    strResult = (result.id == "_" ? "" : result.id + "=") + resultVal;

                    }
                if (result.errors.Length > 0)
                    {
                    strResult += " <<< ";
                    foreach (var err in result.errors)
                        strResult += $"[{err.range.startLine}, {err.range.startPos}, {err.range.endLine}, {err.range.endPos}] {err.message}";
                    }
                strResult += $" {result.text} {strComment}";
                if (strResult.Length > 0)
                    wnd.Output += strResult + "\n";
                }
            }

        //private static void parseWithCSharp(string formula, MainWindow wnd)
        //    {
        //    //Tokenizer tok = new Tokenizer(formula);
        //    Parser parser = new Parser(formula);
        //    var statements = parser.parse();
        //    Resolver resolver = new Resolver(parser);
        //    statements.ForEach(stmt =>
        //    {
        //        var result = resolver.resolve(stmt);
        //        if (result.id != null)
        //            wnd.Output += result.id.StringValue + "=";
        //        if (result.number == Double.NaN)
        //            wnd.Output = "NaN";
        //        else
        //            wnd.Output += result.ToString("0.#######");
        //        wnd.Output += "\n";
        //    });
        //    }
        
        const string localStorageFileName = @"D:\Documents\Programming\WPFandXNA\MathParser\localstorage.txt";
        public MainWindow()
            {
            InitializeComponent();
            Formula = "";
            this.DataContext = this;
            txtFormula.Focus();
            this.Title = "MathParser " + DllInterface.GetVersion();
            if(File.Exists(localStorageFileName))
                this.Formula = File.ReadAllText(localStorageFileName);
            }

        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            File.WriteAllText(localStorageFileName, Formula);
        }
    }
    }
