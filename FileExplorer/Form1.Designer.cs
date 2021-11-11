namespace RUDY
{
    partial class Form1
    {
        /// <summary>
        /// 필수 디자이너 변수입니다.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// 사용 중인 모든 리소스를 정리합니다.
        /// </summary>
        /// <param name="disposing">관리되는 리소스를 삭제해야 하면 true이고, 그렇지 않으면 false입니다.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form 디자이너에서 생성한 코드

        /// <summary>
        /// 디자이너 지원에 필요한 메서드입니다. 
        /// 이 메서드의 내용을 코드 편집기로 수정하지 마세요.
        /// </summary>
        private void InitializeComponent()
        {
            this.treeView1 = new System.Windows.Forms.TreeView();
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.listView1 = new System.Windows.Forms.ListView();
            this.chName = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chModifiedDate = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chFormat = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.chSize = ((System.Windows.Forms.ColumnHeader)(new System.Windows.Forms.ColumnHeader()));
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.label1 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.registrate = new System.Windows.Forms.Button();
            this.cancleRegistrate = new System.Windows.Forms.Button();
            this.scan_period_backup = new System.Windows.Forms.Button();
            this.set_folder = new System.Windows.Forms.Button();
            this.first_backup = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // treeView1
            // 
            this.treeView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.treeView1.Location = new System.Drawing.Point(3, 3);
            this.treeView1.Name = "treeView1";
            this.treeView1.ShowNodeToolTips = true;
            this.treeView1.Size = new System.Drawing.Size(202, 666);
            this.treeView1.TabIndex = 0;
            this.treeView1.BeforeExpand += new System.Windows.Forms.TreeViewCancelEventHandler(this.treeView1_BeforeExpand);
            this.treeView1.AfterSelect += new System.Windows.Forms.TreeViewEventHandler(this.treeView1_AfterSelect);
            // 
            // splitContainer1
            // 
            this.splitContainer1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.splitContainer1.Location = new System.Drawing.Point(12, 75);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.treeView1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.listView1);
            this.splitContainer1.Size = new System.Drawing.Size(933, 672);
            this.splitContainer1.SplitterDistance = 208;
            this.splitContainer1.SplitterWidth = 5;
            this.splitContainer1.TabIndex = 2;
            // 
            // listView1
            // 
            this.listView1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.listView1.Columns.AddRange(new System.Windows.Forms.ColumnHeader[] {
            this.chName,
            this.chModifiedDate,
            this.chFormat,
            this.chSize});
            this.listView1.HideSelection = false;
            this.listView1.Location = new System.Drawing.Point(3, 3);
            this.listView1.Name = "listView1";
            this.listView1.Size = new System.Drawing.Size(706, 666);
            this.listView1.TabIndex = 1;
            this.listView1.UseCompatibleStateImageBehavior = false;
            this.listView1.View = System.Windows.Forms.View.Details;
            this.listView1.DoubleClick += new System.EventHandler(this.listView1_DoubleClick);
            // 
            // chName
            // 
            this.chName.Text = "이름";
            this.chName.Width = 290;
            // 
            // chModifiedDate
            // 
            this.chModifiedDate.Text = "수정한 날짜";
            this.chModifiedDate.Width = 171;
            // 
            // chFormat
            // 
            this.chFormat.Text = "유형";
            this.chFormat.Width = 143;
            // 
            // chSize
            // 
            this.chSize.Text = "크기";
            this.chSize.Width = 101;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(15, 47);
            this.textBox1.Name = "textBox1";
            this.textBox1.Size = new System.Drawing.Size(202, 21);
            this.textBox1.TabIndex = 3;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(15, 13);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(149, 23);
            this.button1.TabIndex = 4;
            this.button1.Text = "상위 디렉토리 설정";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Location = new System.Drawing.Point(223, 47);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(75, 23);
            this.button2.TabIndex = 5;
            this.button2.Text = "검색";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(170, 18);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(0, 12);
            this.label1.TabIndex = 6;
            // 
            // label2
            // 
            this.label2.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(10, 10);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(392, 36);
            this.label2.TabIndex = 8;
            this.label2.Text = "※ 리스트에 나열된 목록 중 하나를 더블클릭 하면 폴더or파일을 실행함.\r\n\r\n※ 경로나 파일 선택 후 백업 버튼 클릭 시 자동 백업 시작\r\n";
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.label2);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 750);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(957, 66);
            this.panel1.TabIndex = 9;
            // 
            // registrate
            // 
            this.registrate.Location = new System.Drawing.Point(664, 7);
            this.registrate.Name = "registrate";
            this.registrate.Size = new System.Drawing.Size(75, 23);
            this.registrate.TabIndex = 10;
            this.registrate.Text = "백업";
            this.registrate.UseVisualStyleBackColor = true;
            this.registrate.Click += new System.EventHandler(this.button3_Click);
            // 
            // cancleRegistrate
            // 
            this.cancleRegistrate.Location = new System.Drawing.Point(783, 7);
            this.cancleRegistrate.Name = "cancleRegistrate";
            this.cancleRegistrate.Size = new System.Drawing.Size(75, 23);
            this.cancleRegistrate.TabIndex = 11;
            this.cancleRegistrate.Text = "백업 중지";
            this.cancleRegistrate.UseVisualStyleBackColor = true;
            this.cancleRegistrate.Click += new System.EventHandler(this.cancleRegistrate_Click);
            // 
            // scan_period_backup
            // 
            this.scan_period_backup.Location = new System.Drawing.Point(521, 7);
            this.scan_period_backup.Name = "scan_period_backup";
            this.scan_period_backup.Size = new System.Drawing.Size(109, 23);
            this.scan_period_backup.TabIndex = 12;
            this.scan_period_backup.Text = "스캔 주기 백업";
            this.scan_period_backup.UseVisualStyleBackColor = true;
            this.scan_period_backup.Click += new System.EventHandler(this.scan_period_backup_Click);
            // 
            // set_folder
            // 
            this.set_folder.Location = new System.Drawing.Point(408, 7);
            this.set_folder.Name = "set_folder";
            this.set_folder.Size = new System.Drawing.Size(75, 23);
            this.set_folder.TabIndex = 13;
            this.set_folder.Text = "선택";
            this.set_folder.UseVisualStyleBackColor = true;
            this.set_folder.Click += new System.EventHandler(this.set_folder_Click);
            // 
            // first_backup
            // 
            this.first_backup.Location = new System.Drawing.Point(0, 0);
            this.first_backup.Name = "first_backup";
            this.first_backup.Size = new System.Drawing.Size(0, 0);
            this.first_backup.TabIndex = 14;
            this.first_backup.Text = "button3";
            this.first_backup.UseVisualStyleBackColor = true;
            this.first_backup.Click += new System.EventHandler(this.first_backup_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(957, 816);
            this.Controls.Add(this.first_backup);
            this.Controls.Add(this.set_folder);
            this.Controls.Add(this.scan_period_backup);
            this.Controls.Add(this.cancleRegistrate);
            this.Controls.Add(this.registrate);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.splitContainer1);
            this.Controls.Add(this.panel1);
            this.MinimumSize = new System.Drawing.Size(520, 480);
            this.Name = "Form1";
            this.Text = "파일 탐색기";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.panel1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TreeView treeView1;
        private System.Windows.Forms.ListView listView1;
        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.ColumnHeader chName;
        private System.Windows.Forms.ColumnHeader chModifiedDate;
        private System.Windows.Forms.ColumnHeader chSize;
        private System.Windows.Forms.ColumnHeader chFormat;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Button registrate;
        private System.Windows.Forms.Button cancleRegistrate;
        private System.Windows.Forms.Button scan_period_backup;
        private System.Windows.Forms.Button set_folder;
        private System.Windows.Forms.Button first_backup;
    }
}

