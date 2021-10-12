using System;
using System.IO;
using Trinet.Core.IO.Ntfs;


class ReadADSStream {
    static void Main(string[] args)
    {
        FileInfo file = new FileInfo("c:\\test\\test.txt");

        //ADS에 있는 스트림 모두 찾음
        foreach (AlternateDataStreamInfo s in file.ListAlternateDataStreams())
        {
            Console.WriteLine("스트림 이름: {0}, 크기: {1} bytes", s.Name, s.Size);

            //스트림 안의 내용 읽고 출력
            AlternateDataStreamInfo adsi =
               file.GetAlternateDataStream(s.Name, FileMode.Open);
            using (TextReader temp_reader = adsi.OpenText())
            {
                Console.WriteLine("{0} 내용: {1}", s.Name, temp_reader.ReadToEnd());
            }

            
        }

        //Zone.Identifier 스트림을 읽어 존재하면 다운로드 파일, 존재하지 않으면 비다운로드 파일
        if (file.AlternateDataStreamExists("Zone.Identifier"))
        {
            Console.WriteLine("다운로드 파일입니다. :");

            AlternateDataStreamInfo s =
               file.GetAlternateDataStream("Zone.Identifier", FileMode.Open);
            using (TextReader reader = s.OpenText())
            {
                Console.WriteLine(reader.ReadToEnd());
            }

            // Delete the stream:
            //s.Delete();
        }
        else
        {
            Console.WriteLine("다운로드 파일이 아닙니다.");
        }

        // Alternative method to delete the stream:
        //file.DeleteAlternateDataStream("Zone.Identifier");
    }
}
