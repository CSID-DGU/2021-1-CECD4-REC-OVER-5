using System;
using System.IO;
using Trinet.Core.IO.Ntfs;
using System.Text;


class ReadADSStream {
    static void Main(string[] args)
    {
        //********** 파일 접근 **********

        FileInfo file = new FileInfo("c:\\test\\4주차.pdf");
        //*******************************



        //********** 파일에 ADS스트림 생성 및 쓰기 **********

        //생성할 ADS스트림 이름
        AlternateDataStreamInfo makeStream = file.GetAlternateDataStream("testgogo", FileMode.Create);

        //ADS 스트림 안에 넣을 문자열
        Byte[] information = new UTF8Encoding(true).GetBytes("드디어된다시발");


        using (FileStream fs = makeStream.OpenWrite())
        {
            fs.Write(information);
        }
        //****************************************************



        //********** 파일에 있는 ADS 접근 및 읽기 **********

        //ADS에 있는 스트림 모두 찾음
        foreach (AlternateDataStreamInfo s in file.ListAlternateDataStreams())
        {
            if (s.Name == "Zone.Identifier") continue;
           
            Console.WriteLine("스트림 이름: {0}, 크기: {1} bytes", s.Name, s.Size);

            //스트림 안의 내용 읽고 출력
            AlternateDataStreamInfo adsi = file.GetAlternateDataStream(s.Name, FileMode.Open);
            using (TextReader temp_reader = adsi.OpenText())
            {
                Console.WriteLine("{0} 내용: {1}", s.Name, temp_reader.ReadToEnd());
            }
            Console.WriteLine();
        }
        //**************************************************



        //********** 다운로드파일인지 확인 **********
        
        //Zone.Identifier 스트림을 읽어 존재하면 다운로드 파일, 존재하지 않으면 비다운로드 파일
        if (file.AlternateDataStreamExists("Zone.Identifier"))
        {
            Console.WriteLine("다운로드 파일입니다. :");

            AlternateDataStreamInfo s = file.GetAlternateDataStream("Zone.Identifier", FileMode.Open);
            using (TextReader reader = s.OpenText())
            {
                Console.WriteLine(reader.ReadToEnd());
            }

        }
        else
        {
            Console.WriteLine("다운로드 파일이 아닙니다.");
        }
        //******************************************





        //스트림 삭제
        //file.DeleteAlternateDataStream("info");
    }
}