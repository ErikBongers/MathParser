
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
    }

public class Error
    {
    public string id { get; set; }
    public string message { get; set; }
    }
