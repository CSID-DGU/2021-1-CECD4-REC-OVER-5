using System;
using System.IO;
using System.Collections.Generic;
using Trinet.Core.IO.Ntfs;

namespace RSCS
{
    class Scanner
    {
        //ver1.0에선 직접 생성한 파일과 다운로드한 파일의 백업 기준이 같음
        static Boolean FirstBackup(int index, List<DateTime> CT, List<DateTime> LWT, List<DateTime> LAT) //First BackUp_Creation File
        { 
            Boolean res;

            //수정시간과 생성시간을 먼저 비교 후, 해당사항이 없다면 최근 접근 시간 1년 이내인지 확인하여 return

            int result = DateTime.Compare(CT[index], LWT[index]);

            DateTime now = DateTime.Now;

            int recentAccess = DateTime.Compare(LAT[index], now.AddYears(-1));


            if(recentAccess < 0) //최근 접근 시간 1년 이내 파일만 백업
            {
                res = false;
                return res;
            }
            else
            {
                if (result < 0) //수정시간이 생성시간보다 최근이면 백업 check Yes -> bool 형식으로 return도 가능
                {
                    res = true;
                    return res;
                }
                else
                {
                    res = false;
                    return res;
                }
            }

           

        }

        
        
        static Boolean PeriodBackup_C(int index, List<DateTime> CT, List<DateTime> LWT, List<DateTime> LAT) //Every Period BackUp for Creation File
        {
            Boolean res = true;
            //스캔 주기별 백업에선 생성된 모든 파일을 Backup
            return res;
        }

        static Boolean PeriodBackup_D(int index, List<DateTime> CT, List<DateTime> LWT, List<DateTime> LAT, List<string> FL, DateTime LastBackUpTime) //Every Period BackUp for Download File, FL : Flag List
        {
            Boolean res;

            if (FL[index] == "Done")
            {
                if (DateTime.Compare(LastBackUpTime, LWT[index]) < 0)
                {
                    res = true;
                    return res;
                }
                else
                {
                    res = false;
                    return res;
                }
            }
            else if (FL[index] == "NOT")
            {
                if (DateTime.Compare(CT[index], LWT[index]) != 0)
                {
                    res = true;
                    return res;
                }
                else
                {
                    res = false;
                    return res;
                }
                    
            }
            else
            {
                res = false;
                return res;
            }

        }


        static void Main(string[] args)
        {
            int cnt = 0;
            string filePath = "C:\\Users\\yuti9\\CL";
            string[] files = Directory.GetFiles(@filePath, "*", SearchOption.AllDirectories);
            string streamName = "rudyflag";

            List<string> fileNameList = new List<string>();
            List<DateTime> fileCreationTimeList = new List<DateTime>();
            List<DateTime> fileLastWriteTimeList = new List<DateTime>();
            List<DateTime> fileLastAccessTimeList = new List<DateTime>();
            List<bool> fileIsDownloadedList = new List<bool>();
            List<string> fileFlagList = new List<string>();

            List<string> backupfilepathList = new List<string>();
            DateTime LastBackUpTime = DateTime.Now.AddHours(-1); //Test용 최근 백업 타임은 1년전


            FileInfo info;
            foreach (var fileName in files)  //insert metadata loop
            {
                info = new FileInfo(fileName);

                fileNameList.Add(fileName);
                fileCreationTimeList.Add(info.CreationTime);
                fileLastWriteTimeList.Add(info.LastWriteTime);
                fileLastAccessTimeList.Add(info.LastAccessTime);

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


            Boolean result;


            //초기 파일 백업
            foreach(bool chk in fileIsDownloadedList)
            {
                result = FirstBackup(cnt, fileCreationTimeList, fileLastWriteTimeList, fileLastAccessTimeList);
                if(result == true)
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
                    if (result == true)
                        backupfilepathList.Add(fileNameList[cnt]);
                }
                else
                {
                    result = PeriodBackup_C(cnt, fileCreationTimeList, fileLastWriteTimeList, fileLastAccessTimeList);
                    if (result == true)
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