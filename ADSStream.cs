using System;
using System.IO;
using Trinet.Core.IO.Ntfs;
using System.Text;


class ADSStream {
    static void Main(string[] args)
    {
        //string filePath = "c:\\test\\test.txt";

        //setADSStream(filePath, "itistest", "teststringhere");

        //string temp1 = getADSStream(filePath, "itistest");

        //Console.WriteLine(temp1);

        //string temp2 = getAllADSStream(filePath);
        //Console.WriteLine(temp2);

        //스트림 삭제
        //file.DeleteAlternateDataStream("testgogo");
    }


    //Zone.Identifier 정보 존재 시 1 반환, 아니면 0 반환
    static int isDownloadFile(string filePath)
    {
        FileInfo file = new FileInfo(filePath);

        if (file.AlternateDataStreamExists("Zone.Identifier"))
            return 1;  
        else 
            return 0;
        
    }


    //ADS스트림에 문자열 삽입
    static void setADSStream(string filePath, string streamName, string str)
    {
        FileInfo file = new FileInfo(filePath);
  
        //생성할 ADS스트림을 지우고 새로 만듦
        file.DeleteAlternateDataStream("RudyFlag");
        AlternateDataStreamInfo makeStream = file.GetAlternateDataStream(streamName, FileMode.Create);

        //ADS 스트림 안에 넣을 문자열 생성
        Byte[] information = new UTF8Encoding(true).GetBytes(str);

        //streamName의 스트림 안에 str 삽입
        using (FileStream fs = makeStream.OpenWrite())      
            fs.Write(information);
        
    }


    //ADS의 스트림 이름을 입력받아 스트림 내용 읽기
    static string getADSStream(string filePath, string streamName)
    {
        FileInfo file = new FileInfo(filePath);

        //해당 스트림이 ADS에 없다면 빈 문자열 반환
        if (!file.AlternateDataStreamExists(streamName))
            return "";

       
        AlternateDataStreamInfo info = file.GetAlternateDataStream(streamName, FileMode.Open);
        using (TextReader reader = info.OpenText())
        {
            return reader.ReadToEnd();
        }
        
    }


    //파일의 모든 ADS 스트림과 내용 읽어와 string으로 반환
    //아무 스트림도 없다면 빈 문자열 반환
    static string getAllADSStream(string filePath)
    {
        FileInfo file = new FileInfo(filePath);

        StringBuilder sb = new StringBuilder("");
        foreach (AlternateDataStreamInfo s in file.ListAlternateDataStreams())
        {
            sb.Append(s.Name);
            sb.Append(":");

            AlternateDataStreamInfo info = file.GetAlternateDataStream(s.Name, FileMode.Open);
            using (TextReader reader = info.OpenText())
            {
                sb.Append(reader.ReadToEnd());
                sb.Append(" ");
            }            
        }
        return sb.ToString();
    }

}

