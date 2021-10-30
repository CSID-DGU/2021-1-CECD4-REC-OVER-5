using System;
using System.IO;
using System.Collections.Generic;
using Trinet.Core.IO.Ntfs;

namespace RUDY_SCANNER
{
    class Scanner
    {
        static void Main(string[] args)
        {
            string filePath = "c:\\test";
            string[] files = Directory.GetFiles(@filePath, "*", SearchOption.AllDirectories);
            string streamName = "rudyflag";

            List<string> fileNameList = new List<string>();
            List<string> fileCreationTimeList = new List<string>();
            List<string> fileLastWriteTimeList = new List<string>();
            List<string> fileLastAccessTimeList = new List<string>();
            List<bool> fileIsDownloadedList = new List<bool>();
            List<string> fileFlagList = new List<string>();

            FileInfo info;
            foreach (var fileName in files)  //insert metadata loop
            {
                info = new FileInfo(fileName);

                fileNameList.Add(fileName);
                fileCreationTimeList.Add(info.CreationTime.ToString("yyyy/MM/dd hh:mm:ss"));
                fileLastWriteTimeList.Add(info.LastWriteTime.ToString("yyyy/MM/dd hh:mm:ss"));
                fileLastAccessTimeList.Add(info.LastAccessTime.ToString("yyyy/MM/dd hh:mm:ss"));

                if (info.AlternateDataStreamExists("Zone.Identifier"))
                    fileIsDownloadedList.Add(true);
                else
                    fileIsDownloadedList.Add(false);

                if (!info.AlternateDataStreamExists(streamName))
                    fileFlagList.Add(" ");
                else
                {
                    AlternateDataStreamInfo ADSinfo = info.GetAlternateDataStream(streamName, FileMode.Open);
                    using (TextReader reader = ADSinfo.OpenText())
                    {
                        fileFlagList.Add(reader.ReadToEnd());
                    }
                }
            }//end of insert metadata loop

            foreach (string str in fileNameList)  //리스트이름 바꿔서 다른정보 출력해 볼 수 있음(다운로드유무만 스트링말고 bool임)
            {
                Console.WriteLine(str);
            }

        }//main end
    }//class end
}
