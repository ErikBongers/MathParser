
public class JsonResults
    {
    public Result[] result { get; set; }
    }

public class Result
    {
    public string id { get; set; }
    public string value { get; set; }
    public string unit { get; set; }
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
