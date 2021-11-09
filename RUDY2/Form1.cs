using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.IO;
using Microsoft.Win32;
using Trinet.Core.IO.Ntfs;
using Google.Apis.Auth.OAuth2;
using Google.Apis.Drive.v3;
using Google.Apis.Services;
using Google.Apis.Util.Store;
using System.Threading;

namespace RUDY2
{
    public partial class Form1 : Form
    {

        static string strAppName = "RUDY2";
        static string streamName = "last_backup_time";

        static string[] Scopes = { DriveService.Scope.Drive };
        static UserCredential credential;
        static string jsonFileName = "C:\\Users\\조영욱\\Desktop\\client_secret_160122161633-46id2122fmqua63pt1jhikqbmo0j0n32.apps.googleusercontent.com.json";


        public Form1()
        {
            InitializeComponent();
            this.scan_button.Click += new System.EventHandler(this.scan_button_click);
            this.upload_button.Click += new System.EventHandler(this.upload_button_click);

            this.first_backup.Click += new System.EventHandler(this.first_backup_click);
            this.scan_period_backup.Click += new System.EventHandler(this.scan_period_backup_click);

            this.set_folder.Click += new System.EventHandler(this.set_folder_click);
            this.registrate.Click += new System.EventHandler(this.registrate_click);
            this.cancleRegistrate.Click += new System.EventHandler(this.cancleRegistrate_click);
        }

        static void setADSStream(string filePath, string streamName, string str)
        {
            FileInfo file = new FileInfo(filePath);

            //생성할 ADS스트림을 지우고 새로 만듦
            file.DeleteAlternateDataStream(streamName);
            AlternateDataStreamInfo makeStream = file.GetAlternateDataStream(streamName, FileMode.Create);

            //ADS 스트림 안에 넣을 문자열 생성
            Byte[] information = new UTF8Encoding(true).GetBytes(str);

            //streamName의 스트림 안에 str 삽입
            using (FileStream fs = makeStream.OpenWrite())
                fs.Write(information, 0, information.Length);

        }

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


        static void scanFileList(string filePath, string streamName, List<string> fileNameList, List<DateTime> fileCreationTimeList, List<DateTime> fileLastWriteTimeList,
                List<DateTime> fileLastAccessTimeList, List<bool> fileIsDownloadedList, List<string> fileFlagList)
        {
            FileInfo info;
            foreach (var fileName in Directory.GetFiles(@filePath, "*", SearchOption.AllDirectories))  //insert metadata loop
            {
                info = new FileInfo(fileName);

                fileNameList.Add(fileName);
                fileCreationTimeList.Add(info.CreationTime);
                fileLastWriteTimeList.Add(info.LastWriteTime);
                fileLastAccessTimeList.Add(info.LastAccessTime);

                //download
                if (info.AlternateDataStreamExists("Zone.Identifier"))
                    fileIsDownloadedList.Add(true);
                else
                    fileIsDownloadedList.Add(false);


                //flag(last backup time)
                if (!info.AlternateDataStreamExists(streamName))
                    //업로드 이력 없으면 플래그리스트에 NOT 입력
                    fileFlagList.Add("NOT");
                else
                {
                    AlternateDataStreamInfo ADSinfo = info.GetAlternateDataStream(streamName, FileMode.Open);
                    using (TextReader reader = ADSinfo.OpenText())
                    {
                        //업로드 이력 있으면 플래그리스트에 최근업로드날짜 입력
                        fileFlagList.Add(reader.ReadToEnd());
                    }
                }
            }//end of insert metadata loop
        }


        



        private void scan_button_click(object sender, EventArgs e)
        {
            if (Properties.Settings.Default.folder == "")
            {
                MessageBox.Show("폴더를 선택해주세요");
                return;
            }

            MessageBox.Show("마지막 스캔 실행: " + Properties.Settings.Default.lastScanDate);

            List<string> fileNameList = new List<string>();
            List<DateTime> fileCreationTimeList = new List<DateTime>();
            List<DateTime> fileLastWriteTimeList = new List<DateTime>();
            List<DateTime> fileLastAccessTimeList = new List<DateTime>();
            List<bool> fileIsDownloadedList = new List<bool>();
            List<string> fileFlagList = new List<string>();
            scanFileList(Properties.Settings.Default.folder, streamName, fileNameList, fileCreationTimeList, fileLastWriteTimeList,
                fileLastAccessTimeList, fileIsDownloadedList, fileFlagList);
            foreach (string str in fileNameList)  //리스트이름 바꿔서 다른정보 출력해 볼 수 있음(다운로드유무만 스트링말고 bool임)
            {
                //MessageBox.Show(str);
            }

            Properties.Settings.Default.lastScanDate = DateTime.Now;
            Properties.Settings.Default.Save();
        }

        private void upload_button_click(object sender, EventArgs e)  //구글드라이브 업로드
        {
            if (Properties.Settings.Default.folder == "")
            {
                MessageBox.Show("폴더를 선택해주세요");
                return;
            }

            List<string> fileNameList = new List<string>();
            List<DateTime> fileCreationTimeList = new List<DateTime>();
            List<DateTime> fileLastWriteTimeList = new List<DateTime>();
            List<DateTime> fileLastAccessTimeList = new List<DateTime>();
            List<bool> fileIsDownloadedList = new List<bool>();
            List<string> fileFlagList = new List<string>();
            scanFileList(Properties.Settings.Default.folder, streamName, fileNameList, fileCreationTimeList, fileLastWriteTimeList,
                fileLastAccessTimeList, fileIsDownloadedList, fileFlagList);

            foreach (string fileName in fileNameList)  
            {
                
                googleDriveUpload(fileName);
            }
            

            MessageBox.Show("업로드 실행.\n구글드라이브 업로드 완료.");
        }

        private void first_backup_click(object sender, EventArgs e)
        {
            if (Properties.Settings.Default.folder == "")
            {
                MessageBox.Show("폴더를 선택해주세요");
                return;
            }

            List<string> fileNameList = new List<string>();
            List<DateTime> fileCreationTimeList = new List<DateTime>();
            List<DateTime> fileLastWriteTimeList = new List<DateTime>();
            List<DateTime> fileLastAccessTimeList = new List<DateTime>();
            List<bool> fileIsDownloadedList = new List<bool>();
            List<string> fileFlagList = new List<string>();
            scanFileList(Properties.Settings.Default.folder, streamName, fileNameList, fileCreationTimeList, fileLastWriteTimeList,
                fileLastAccessTimeList, fileIsDownloadedList, fileFlagList);

            

            int cnt = 0;
            foreach (string file in fileNameList)
            {
                if(FirstBackup(cnt, fileCreationTimeList, fileLastWriteTimeList, fileLastAccessTimeList))
                {
                    googleDriveUpload(file);
                    
                }
                cnt++;
            }

            Properties.Settings.Default.lastScanDate = DateTime.Now;  //마지막스캔일 저장
            Properties.Settings.Default.afterFistBackUp = true;  //first backup 했다고 저장
            Properties.Settings.Default.Save();

        }


        private void scan_period_backup_click(object sender, EventArgs e)
        {
            if (Properties.Settings.Default.folder == "")
            {
                MessageBox.Show("폴더를 선택해주세요");
                return;
            }

            List<string> fileNameList = new List<string>();
            List<DateTime> fileCreationTimeList = new List<DateTime>();
            List<DateTime> fileLastWriteTimeList = new List<DateTime>();
            List<DateTime> fileLastAccessTimeList = new List<DateTime>();
            List<bool> fileIsDownloadedList = new List<bool>();
            List<string> fileFlagList = new List<string>();
            scanFileList(Properties.Settings.Default.folder, streamName, fileNameList, fileCreationTimeList, fileLastWriteTimeList,
                fileLastAccessTimeList, fileIsDownloadedList, fileFlagList);

            Boolean result;
            int cnt = 0;
            foreach (bool chk in fileIsDownloadedList)
            {
                if (chk == true)
                {
                    result = PeriodBackup_D(cnt, fileCreationTimeList, fileLastWriteTimeList, fileFlagList);

                    if (result == true)
                    {
                        
                        googleDriveUpload(fileNameList[cnt]);
                    }
                }
                else
                {

                    result = PeriodBackup_C(cnt, fileCreationTimeList, fileFlagList);
                    if (result == true)
                    {
                        googleDriveUpload(fileNameList[cnt]);
                    }
                }
                cnt++;
            }
            

            Properties.Settings.Default.lastScanDate = DateTime.Now;  //마지막스캔일 저장
            Properties.Settings.Default.Save();


        }


        private void set_folder_click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                Properties.Settings.Default.folder = fbd.SelectedPath;
                Properties.Settings.Default.afterFistBackUp = false;
                Properties.Settings.Default.Save();
                MessageBox.Show(Properties.Settings.Default.folder + " 폴더의 파일을 자동 스캔/업로드 합니다.");
            }
        }
        
        private void registrate_click (object sender, EventArgs e)
        {
            
            using (RegistryKey rk = Registry.CurrentUser.OpenSubKey(@"SOFTWARE\Microsoft\Windows\CurrentVersion\Run", true))
            {
                try
                {
                    //레지스트리 등록...
                    if (rk.GetValue(strAppName) == null)
                    {
                        rk.SetValue(strAppName, Application.ExecutablePath.ToString());
                    }

                    //레지스트리 닫기...
                    rk.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("오류: " + ex.Message.ToString());
                }

                if (Properties.Settings.Default.folder == "")
                {
                    MessageBox.Show("먼저 폴더를 선택해주세요");
                    return;
                }

                

                MessageBox.Show("이제 컴퓨터가 켜지면 프로그램이 자동으로 켜지고\n" +
                    "주기마다 자동 스캔/업로드가 진행됩니다.");

                /* 여기
                MessageBox.Show("첫 번째 업로드를 시작합니다.");
                first_backup_click(null, null);
                */
            }
        }



        private void cancleRegistrate_click(object sender, EventArgs e)
        {
            using (RegistryKey rk = Registry.CurrentUser.OpenSubKey(@"SOFTWARE\Microsoft\Windows\CurrentVersion\Run", true))
            {

                try
                {
                    //레지스트리 삭제
                    if (rk.GetValue(strAppName) != null)
                    {
                        rk.DeleteValue(strAppName, false);
                    }


                    //레지스트리 닫기...
                    rk.Close();
                }
                catch (Exception ex)
                {
                    MessageBox.Show("오류: " + ex.Message.ToString());
                }

                MessageBox.Show("이제 프로그램이 자동으로 켜지지 않고\n" +
                    "자동 스캔/업로드를 진행하지 않습니다.");
               
            }
        }


        //툴 켜졌을때 자동실행
        private void Form1_Load(object sender, EventArgs e)
        {
            if (Properties.Settings.Default.afterFistBackUp == true)  //첫 백업 한 이후 실행
            {
                //마지막 백업 실행 이후 24시간이 지났으면 스캔주기 백업 자동 실행
                DateTime current = DateTime.Now;
                TimeSpan ts = new TimeSpan(Properties.Settings.Default.lastScanDate.Hour + 24,
                    Properties.Settings.Default.lastScanDate.Minute, Properties.Settings.Default.lastScanDate.Second);
                DateTime oneDayAfterlastScanDate = Properties.Settings.Default.lastScanDate.Date + ts;

                if (current > oneDayAfterlastScanDate)
                    scan_period_backup_click(null, null);
            }
        }
       

        private void Form1_MouseClick(object sender, EventArgs e)
        {

            
        }


        static DriveService makeService()
        {
            using (var stream =
                new FileStream(jsonFileName, FileMode.Open, FileAccess.Read))
            {
                string credPath = System.Environment.GetFolderPath(System.Environment.SpecialFolder.Personal);
                credPath = Path.Combine(credPath, ".credentials/rudy.json");
                credential = GoogleWebAuthorizationBroker.AuthorizeAsync(
                    GoogleClientSecrets.Load(stream).Secrets,
                    Scopes,
                    "user",
                    CancellationToken.None,
                    new FileDataStore(credPath, true)).Result;
                Console.WriteLine("Credential file saved to: " + credPath);
            }

            // Create Drive API service.
            var service = new DriveService(new BaseClientService.Initializer()
            {
                HttpClientInitializer = credential,
                ApplicationName = "rudy",
            });

            return service;
        }

        static void googleDriveUpload(string filePath)
        {
            string path = filePath;

            //ads스트림에 업로드날짜 세팅
            setADSStream(path, streamName, DateTime.Now.ToString("yyyyMMddHHmmss"));

            var fileMetadata = new Google.Apis.Drive.v3.Data.File();

            fileMetadata.Name = Path.GetFileName(path);
            fileMetadata.MimeType = "application/octet-stream";


            FilesResource.CreateMediaUpload request;
            using (var stream = new System.IO.FileStream(path, System.IO.FileMode.Open))
            {
                request = makeService().Files.Create(fileMetadata, stream, fileMetadata.MimeType);
                request.Fields = "id";
                request.Upload();

            }
        }


        //ver1.0에선 직접 생성한 파일과 다운로드한 파일의 백업 기준이 같음
        static Boolean FirstBackup(int index, List<DateTime> CT, List<DateTime> LWT, List<DateTime> LAT) //First BackUp_Creation File
        {
            Boolean res;

            //수정시간과 생성시간을 먼저 비교 후, 해당사항이 없다면 최근 접근 시간 1년 이내인지 확인하여 return

            int result = DateTime.Compare(CT[index], LWT[index]);

            DateTime now = DateTime.Now;

            int recentAccess = DateTime.Compare(LAT[index], now.AddYears(-1));


            if (recentAccess < 0) //최근 접근 시간 1년 이내 파일만 백업
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



        static Boolean PeriodBackup_C(int index, List<DateTime> fileCreateTimeList, List<string> FL) //Every Period BackUp for Creation File
        {      
            if (FL[index] == "NOT")
            {
                return true;
            }

            //생성일보다 마지막 백업 시점이 최근이면 백업 안 함
            
            if (DateTime.ParseExact(FL[index], "yyyyMMddHHmmss", null) > fileCreateTimeList[index]) {
                return false;
            }

            return true;
        }

        static Boolean PeriodBackup_D(int index, List<DateTime> CT, List<DateTime> LWT, List<string> FL) //Every Period BackUp for Download File, FL : Flag List
        {
            if (FL[index] != "NOT")
            {

                if (DateTime.ParseExact(FL[index], "yyyyMMddHHmmss", null) < LWT[index].AddHours(-1))
                {
                    return true;
                }
                else
                {


                    return false;
                }              
            }
            else if (FL[index] == "NOT")
            {
                if (DateTime.Compare(CT[index], LWT[index]) != 0)
                {
                    return true;
                }
                else
                {
                    return false;
                }

            }
            else
            {
                return  false;
            }

        }








    }
}
