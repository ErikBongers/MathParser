﻿
public class JsonResults
    {
    public Result[] result { get; set; }
    }

public class Result
    {
    public string id { get; set; }
    public string type { get; set; }
    public Number number { get; set; }
    public Date date { get; set; }
    public Duration duration{ get; set; }
    public Error[] errors { get; set; }
    public string text { get; set; }
    public string comment { get; set; }
    public bool onlyComment { get; set; }
    public bool mute { get; set; }
    }

public class Error
    {
    public string id { get; set; }
    public string message { get; set; }
    public Range range { get; set; }
    }

public class Number
    {
    public string val { get; set; }
    public string sig { get; set; }
    public double exp { get; set; }
    public string fmt { get; set; }
    public string fmtd { get; set; }
    public string u { get; set; }
    }

public class Date
{
    public string formatted { get; set; }
}

public class Duration
    {
    public string years { get; set; }
    public string months{ get; set; }
    public string days { get; set; }
    }

public class Range
    {
    public long startLine { get; set; }
    public long startPos { get; set; }
    public long endLine { get; set; }
    public long endPos { get; set; }
    }