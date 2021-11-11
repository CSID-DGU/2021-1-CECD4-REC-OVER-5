using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;
using Microsoft.Win32;
using Trinet.Core.IO.Ntfs;
using Google.Apis.Auth.OAuth2;
using Google.Apis.Drive.v3;
using Google.Apis.Services;
using Google.Apis.Util.Store;


//namespace FileExplorer
namespace RUDY
{
    public partial class Form1 : Form
    {

        static string strAppName = "RUDY2";
        static string streamName = "last_backup_time";

        static string[] Scopes = { DriveService.Scope.Drive };
        static UserCredential credential;
        static string jsonFileName = "C:\\Users\\yuti9\\client_secret_412671745934-s2muv6p3i0m9f9igdpa5n1uac2plquvb.apps.googleusercontent.com.json";

        string m_curPath = "";
        Thread m_thread;

        string backup_path = "";

        public Form1()
        {
            InitializeComponent();
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

            if (DateTime.ParseExact(FL[index], "yyyyMMddHHmmss", null) > fileCreateTimeList[index])
            {
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
                return false;
            }

        }

        private void Form1_Load(object sender, EventArgs e)
        {
            m_curPath = "Root";
            label1.Text = m_curPath;

            TreeNode root = treeView1.Nodes.Add(m_curPath);

            string[] drives = Directory.GetLogicalDrives();

            foreach (string drive in drives)
            {
                DriveInfo di = new DriveInfo(drive);

                if (di.IsReady)
                {
                    TreeNode node = root.Nodes.Add(drive);
                    node.Nodes.Add("\\");
                }
            }

            if (FileExplorer.Properties.Settings.Default.afterFistBackUp == true)  //첫 백업 한 이후 실행
            {
                //마지막 백업 실행 이후 24시간이 지났으면 스캔주기 백업 자동 실행
                DateTime current = DateTime.Now;
                TimeSpan ts = new TimeSpan(FileExplorer.Properties.Settings.Default.lastScanDate.Hour + 24,
                    FileExplorer.Properties.Settings.Default.lastScanDate.Minute, FileExplorer.Properties.Settings.Default.lastScanDate.Second);
                DateTime oneDayAfterlastScanDate = FileExplorer.Properties.Settings.Default.lastScanDate.Date + ts;

                if (current > oneDayAfterlastScanDate)
                    scan_period_backup_Click(null, null);
            }
        }

        private void treeView1_BeforeExpand(object sender, TreeViewCancelEventArgs e)
        {
            try
            {
                if (e.Node.Nodes.Count == 1 && e.Node.Nodes[0].Text.Equals("\\"))
                {
                    e.Node.Nodes.Clear();

                    string path = e.Node.FullPath.Substring(e.Node.FullPath.IndexOf("\\") + 1);

                    string[] directories = Directory.GetDirectories(path);
                    foreach (string directory in directories)
                    {
                        TreeNode newNode = e.Node.Nodes.Add(directory.Substring(directory.LastIndexOf("\\") + 1));
                        newNode.Nodes.Add("\\" );
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("treeView1_BeforeExpand : " + ex.Message);
            }
        }

        private void ViewDirectoryList(string path)
        {
            if (m_thread != null && m_thread.IsAlive)
                m_thread.Abort();

            string curPath = path;

            Console.WriteLine(path.IndexOf("Root\\"));
            if (path.IndexOf("Root\\") == 0)
            {
                curPath = path.Substring(path.IndexOf("\\") + 1);
                label1.Text = (curPath.Length > 4) ? curPath.Remove(curPath.IndexOf("\\") + 1, 1) : curPath;
                m_curPath = label1.Text;
                backup_path = m_curPath;
            }
            else
            {
                label1.Text = path;
                m_curPath = path;
                backup_path = path;
            }

            try
            {
                listView1.Items.Clear();

                string[] directories = Directory.GetDirectories(curPath);

                foreach (string directory in directories)
                {
                    DirectoryInfo info = new DirectoryInfo(directory);
                    ListViewItem item = new ListViewItem(new string[]
                    {
                        info.Name, info.LastWriteTime.ToString(), "파일 폴더", ""
                    });
                    listView1.Items.Add(item);
                }

                string[] files = Directory.GetFiles(curPath);

                foreach (string file in files)
                {
                    FileInfo info = new FileInfo(file);
                    ListViewItem item = new ListViewItem(new string[]
                    {
                        info.Name, info.LastWriteTime.ToString(), info.Extension, ((info.Length/1000)+1).ToString()+"KB"
                    });
                    listView1.Items.Add(item);
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine("ViewDirectoryList : " + ex.Message);
            }
        }

        private void SelectTreeView(TreeNode node)
        {
            if (node.FullPath == null)
            {
                Console.WriteLine("empth node.FullPath");
                return;
            }

            string path = node.FullPath;

            ViewDirectoryList(path);
        }

        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            SelectTreeView(e.Node);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog fbd = new FolderBrowserDialog();

            if (fbd.ShowDialog() == DialogResult.OK)
            {
                m_curPath = fbd.SelectedPath;
                Console.WriteLine(m_curPath);
                label1.Text = m_curPath;
                backup_path = m_curPath;

                ViewDirectoryList(m_curPath);
            }
        }

        private void listView1_DoubleClick(object sender, EventArgs e)
        {
            if (listView1.SelectedItems.Count == 1)
            {
                string processPath;
                if (listView1.SelectedItems[0].Text.IndexOf("\\") > 0)
                    processPath = listView1.SelectedItems[0].Text;
                else
                    processPath = m_curPath + "\\" + listView1.SelectedItems[0].Text;

                Process.Start("explorer.exe", processPath);
            }
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if(m_thread != null && m_thread.IsAlive)
                m_thread.Abort();

            m_curPath = label1.Text;
            DirectoryInfo rootDirInfo = new DirectoryInfo(m_curPath);
            string searchFiles = textBox1.Text;

            m_thread = new Thread(delegate ()
            {
                WalkDirectoryTree(rootDirInfo, searchFiles);
            });
            m_thread.Start();
        }

        public void WalkDirectoryTree(DirectoryInfo dirInfo, string searchFiles)
        {
            listView1.Items.Find(searchFiles, true);

            FileInfo[] files = null;
            DirectoryInfo[] subDirs = null;
           
            try
            {
                files = dirInfo.GetFiles(searchFiles + "*.*");
            }
            catch (UnauthorizedAccessException e)
            {
                Console.WriteLine(e.Message);
            }
            catch (DirectoryNotFoundException e)
            {
                Console.WriteLine(e.Message);
            }

            if (files != null)
            {
                DirectoryInfo tempDirInfo = new DirectoryInfo(m_curPath);

                if (dirInfo.ToString() == tempDirInfo.ToString())
                    listView1.Items.Clear();

                foreach (FileInfo fi in files)
                {
                    ListViewItem item = new ListViewItem(new string[]
                    {
                        fi.FullName, fi.LastWriteTime.ToString(), fi.Extension, ((fi.Length/1000)+1).ToString()+"KB"
                    });
                    listView1.Items.Add(item);
                }

                subDirs = dirInfo.GetDirectories();
                foreach (DirectoryInfo di in subDirs)
                {
                    WalkDirectoryTree(di, searchFiles);
                }
            }
        }

        private void button3_Click(object sender, EventArgs e) //백업 버튼
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

                if (FileExplorer.Properties.Settings.Default.folder == "")
                {
                    MessageBox.Show("먼저 폴더를 선택해주세요");
                    return;
                }



                MessageBox.Show("이제 컴퓨터가 켜지면 프로그램이 자동으로 켜지고\n" +
                    "주기마다 자동 스캔/업로드가 진행됩니다.");


                MessageBox.Show("첫 번째 업로드를 시작합니다.");
                first_backup_Click(null, null);

            }
        }

        private void scan_period_backup_Click(object sender, EventArgs e)
        {
            if (FileExplorer.Properties.Settings.Default.folder == "")
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
            scanFileList(FileExplorer.Properties.Settings.Default.folder, streamName, fileNameList, fileCreationTimeList, fileLastWriteTimeList,
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


            FileExplorer.Properties.Settings.Default.lastScanDate = DateTime.Now;  //마지막스캔일 저장
            FileExplorer.Properties.Settings.Default.Save();
        }

        private void set_folder_Click(object sender, EventArgs e)
        {
            FileExplorer.Properties.Settings.Default.folder = backup_path;
            FileExplorer.Properties.Settings.Default.afterFistBackUp = false;
            FileExplorer.Properties.Settings.Default.Save();
            MessageBox.Show(FileExplorer.Properties.Settings.Default.folder + " 폴더의 파일을 자동 스캔/업로드 합니다.");
        }

        private void first_backup_Click(object sender, EventArgs e)
        {
            if (FileExplorer.Properties.Settings.Default.folder == "")
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
            scanFileList(FileExplorer.Properties.Settings.Default.folder, streamName, fileNameList, fileCreationTimeList, fileLastWriteTimeList,
                fileLastAccessTimeList, fileIsDownloadedList, fileFlagList);



            int cnt = 0;
            foreach (string file in fileNameList)
            {
                if (FirstBackup(cnt, fileCreationTimeList, fileLastWriteTimeList, fileLastAccessTimeList))
                {
                    googleDriveUpload(file);

                }
                cnt++;
            }

            FileExplorer.Properties.Settings.Default.lastScanDate = DateTime.Now;  //마지막스캔일 저장
            FileExplorer.Properties.Settings.Default.afterFistBackUp = true;  //first backup 했다고 저장
            FileExplorer.Properties.Settings.Default.Save();
        }

        private void cancleRegistrate_Click(object sender, EventArgs e)
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
    }
}
