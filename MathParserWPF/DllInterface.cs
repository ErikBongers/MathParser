﻿using System.Runtime.InteropServices;
using System.Text;

namespace MathParserWPF 
    {

    public static class DllInterface
        {
        private const string DllFilePath = @"D:\Documents\Programming\WPFandXNA\MathParser\x64\Debug\MathParserDll.dll";

        [DllImport(DllFilePath, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
        private extern static int parse(string str);

        public static int Parse(string str)
            {
            return parse(str);
            }

        [DllImport(DllFilePath, CallingConvention = CallingConvention.Cdecl)]
        private extern static int getResultLen();

        public static int GetResultLen() { return getResultLen(); }

        [DllImport(DllFilePath, CallingConvention = CallingConvention.Cdecl)]
        private extern static void getResult(StringBuilder buf, int strlen);

        public static string GetResult()
            {
            int strlen = getResultLen();
            StringBuilder buf = new StringBuilder(strlen);
            getResult(buf, strlen);
            return buf.ToString();
            }

        [DllImport(DllFilePath, CallingConvention = CallingConvention.Cdecl)]
        private extern static void getVersion(StringBuilder buf, int strlen);

        public static string GetVersion()
            {
            StringBuilder buf = new StringBuilder(100);
            getVersion(buf, 100);
            return buf.ToString();
            }

        }

    }