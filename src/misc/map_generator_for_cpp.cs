// "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\MSBuild\15.0\Bin\Roslyn\csc.exe" map_generator_for_cpp.cs

using System;
using System.IO;
using System.Linq;
using System.Text;

namespace Misc {
    class Program {
        static string GetRepr(char c){
            return Uri.HexEscape(c).Replace("%", "\\x");
        }
        static string GetPrint(char c){
            if (c == '\\') { return "\\\\"; }
            if (c == '"') { return "\\\""; }
            return c.ToString();
        }
        static void Main(){
            using (var stream = new FileStream(@"SG0_Charset.utf8", FileMode.Open, FileAccess.Read))
                using (var reader = new StreamReader(stream, Encoding.UTF8))
                    {
                        int i = 0;
                        foreach (var c in reader.ReadToEnd())
                        {
                            byte[] myBytes = Encoding.UTF8.GetBytes($"{c}");
                            System.Console.WriteLine($"lookup.emplace(0x{i,-4:x}, \"" + string.Join("", myBytes.Select(b => b > 127 ? 
                            GetRepr((char)b) : GetPrint((char)b))) + "\");");
                            ++i;
                        }
                    }
        }
    }
}