using System;
using System.IO;
using System.Collections.Generic;
using Trinet.Core.IO.Ntfs;

namespace RSCS
{
    class Scanner
    {
        //ver1.0에선 직접 생성한 파일과 다운로드한 파일의 백업 기준이 같음
        static string FirstBackup(int index, List<string> CT, List<string> LWT, List<string> LAT) //First BackUp_Creation File
        {
            string r1 = "Y";
            string r2 = "N";


            //수정시간과 생성시간을 먼저 비교 후, 해당사항이 없다면 최근 접근 시간 1년 이내인지 확인하여 return
            DateTime CreationTime = DateTime.ParseExact(CT[index], "yyyyMMddhhmmss", null);
            DateTime LastWriteTime = DateTime.ParseExact(LWT[index], "yyyyMMddhhmmss", null);

            int result = DateTime.Compare(CreationTime, LastWriteTime);

            DateTime LastAccessTime = DateTime.ParseExact(LAT[index], "yyyyMMddhhmmss", null);
            DateTime now = DateTime.Now;

            int recentAccess = DateTime.Compare(LastAccessTime, now.AddYears(-1));


            if(recentAccess < 0) //최근 접근 시간 1년 이내 파일만 백업
            {
                return r2;
            }
            else
            {
                if (result < 0) //수정시간이 생성시간보다 최근이면 백업 check Yes -> bool 형식으로 return도 가능
                    return r1;
                else
                    return r2;
            }

           

        }

        
        
        static string PeriodBackup_C(int index, List<string> CT, List<string> LWT, List<string> LAT) //Every Period BackUp for Creation File
        {
            string r1 = "Y";
            
            //스캔 주기별 백업에선 생성된 모든 파일을 Backup
            return r1;
        }

        static string PeriodBackup_D(int index, List<string> CT, List<string> LWT, List<string> LAT, List<string> FL, DateTime LastBackUpTime) //Every Period BackUp for Download File, FL : Flag List
        {
            string r1 = "Y";
            string r2 = "N";

            DateTime CreationTime = DateTime.ParseExact(CT[index], "yyyyMMddhhmmss", null);
            DateTime LastWriteTime = DateTime.ParseExact(LWT[index], "yyyyMMddhhmmss", null);


            if (FL[index] == "Done")
            {
                if (DateTime.Compare(LastBackUpTime, LastWriteTime) < 0)
                {
                    return r1;
                }
                else
                    return r2;
            }
            else if (FL[index] == "NOT")
            {
                if (DateTime.Compare(CreationTime, LastWriteTime) != 0)
                {
                    return r1;
                }
                else
                    return r2;
            }
            else
                return r2;

        }


        static void Main(string[] args)
        {
            int cnt = 0;
            string filePath = "C:\\Users\\yuti9\\CL";
            string[] files = Directory.GetFiles(@filePath, "*", SearchOption.AllDirectories);
            string streamName = "rudyflag";

            List<string> fileNameList = new List<string>();
            List<string> fileCreationTimeList = new List<string>();
            List<string> fileLastWriteTimeList = new List<string>();
            List<string> fileLastAccessTimeList = new List<string>();
            List<bool> fileIsDownloadedList = new List<bool>();
            List<string> fileFlagList = new List<string>();

            List<string> backupfilepathList = new List<string>();
            DateTime LastBackUpTime = DateTime.Now.AddHours(-1); //Test용 최근 백업 타임은 1년전


            FileInfo info;
            foreach (var fileName in files)  //insert metadata loop
            {
                info = new FileInfo(fileName);

                fileNameList.Add(fileName);
                fileCreationTimeList.Add(info.CreationTime.ToString("yyyyMMddhhmmss"));
                fileLastWriteTimeList.Add(info.LastWriteTime.ToString("yyyyMMddhhmmss"));
                fileLastAccessTimeList.Add(info.LastAccessTime.ToString("yyyyMMddhhmmss"));

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

//            foreach (string str in fileNameList)  //리스트이름 바꿔서 다른정보 출력해 볼 수 있음(다운로드유무만 스트링말고 bool임)
            foreach(string str in fileCreationTimeList)
            {
//                Console.WriteLine(str);
            }
            string result;


            //초기 파일 백업
            foreach(bool chk in fileIsDownloadedList)
            {
                result = FirstBackup(cnt, fileCreationTimeList, fileLastWriteTimeList, fileLastAccessTimeList);
                if(result == "Y")
                    backupfilepathList.Add(fileNameList[cnt]);
                cnt++;
            }

            /*
             * 스캔 주기마다 백업
             * 
            foreach(bool chk in fileIsDownloadedList)
            {
                if (chk == true)
                {
                    result = PeriodBackup_D(cnt, fileCreationTimeList, fileLastWriteTimeList, fileLastAccessTimeList, fileFlagList, LastBackUpTime);
                    if (result == "Y")
                        backupfilepathList.Add(fileNameList[cnt]);
                }
                else
                {
                    result = PeriodBackup_C(cnt, fileCreationTimeList, fileLastWriteTimeList, fileLastAccessTimeList);
                    if (result == "Y")
                        backupfilepathList.Add(fileNameList[cnt]);
                }
                cnt++;
            }

            */
            foreach(string str in backupfilepathList)
            {
                Console.WriteLine("백업해야하는 파일" + str);
            }
            
            

            Console.ReadLine();
        }//main end

        
    }//class end
}