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
        static string streamName = "rudyflag";

        static string[] Scopes = { DriveService.Scope.Drive };
        static UserCredential credential;
        static string jsonFileName = "C:\\Users\\조영욱\\Desktop\\client_secret_160122161633-46id2122fmqua63pt1jhikqbmo0j0n32.apps.googleusercontent.com.json";


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
        }


        public Form1()
        {
            InitializeComponent();
            this.scan_button.Click += new System.EventHandler(this.scan_button_click);
            this.upload_button.Click += new System.EventHandler(this.upload_button_click);
            this.set_folder.Click += new System.EventHandler(this.set_folder_click);
            this.registrate.Click += new System.EventHandler(this.registrate_click);
            this.cancleRegistrate.Click += new System.EventHandler(this.cancleRegistrate_click);
            
            

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

        private void upload_button_click(object sender, EventArgs e)
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


        private void set_folder_click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();
            if (fbd.ShowDialog() == DialogResult.OK)
            {
                Properties.Settings.Default.folder = fbd.SelectedPath;
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

                MessageBox.Show("이제 컴퓨터가 켜지면 프로그램이 자동으로 켜지고\n" +
                    "주기마다 자동 스캔/업로드가 진행됩니다.");

                
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

        private void Form1_Load(object sender, EventArgs e)
        {          
            //마지막 스캔 실행 이후 24시간이 지났으면 스캔 자동 실행
            DateTime current = DateTime.Now;
            TimeSpan ts = new TimeSpan(Properties.Settings.Default.lastScanDate.Hour + 24,
                Properties.Settings.Default.lastScanDate.Minute, Properties.Settings.Default.lastScanDate.Second);
            DateTime oneDayAfterlastScanDate = Properties.Settings.Default.lastScanDate.Date + ts;

            if (current > oneDayAfterlastScanDate)
                scan_button_click(null, null);
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
            Console.WriteLine("업로드 완료");
        }


    }
}
