
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
    public int line { get; set; }
    public int pos { get; set; }
    }

public class Number
    {
    public string value { get; set; }
    public string significand { get; set; }
    public double exponent { get; set; }
    public string format { get; set; }
    public string formatted { get; set; }
    public string unit { get; set; }
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