
namespace RUDY2
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
            this.scan_button = new System.Windows.Forms.Button();
            this.upload_button = new System.Windows.Forms.Button();
            this.rudy_label = new System.Windows.Forms.Label();
            this.set_folder = new System.Windows.Forms.Button();
            this.registrate = new System.Windows.Forms.Button();
            this.cancleRegistrate = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // scan_button
            // 
            this.scan_button.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.scan_button.Location = new System.Drawing.Point(80, 120);
            this.scan_button.Name = "scan_button";
            this.scan_button.Size = new System.Drawing.Size(94, 30);
            this.scan_button.TabIndex = 0;
            this.scan_button.Text = "Scan";
            this.scan_button.UseVisualStyleBackColor = true;
            // 
            // upload_button
            // 
            this.upload_button.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.upload_button.Location = new System.Drawing.Point(80, 180);
            this.upload_button.Name = "upload_button";
            this.upload_button.Size = new System.Drawing.Size(150, 31);
            this.upload_button.TabIndex = 1;
            this.upload_button.Text = "Google Drive Upload";
            this.upload_button.UseVisualStyleBackColor = true;
            // 
            // rudy_label
            // 
            this.rudy_label.AutoSize = true;
            this.rudy_label.Font = new System.Drawing.Font("맑은 고딕", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.rudy_label.Location = new System.Drawing.Point(75, 9);
            this.rudy_label.Name = "rudy_label";
            this.rudy_label.Size = new System.Drawing.Size(128, 30);
            this.rudy_label.TabIndex = 2;
            this.rudy_label.Text = "RUDY TOOL";
            // 
            // set_folder
            // 
            this.set_folder.Font = new System.Drawing.Font("맑은 고딕", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(129)));
            this.set_folder.Location = new System.Drawing.Point(80, 61);
            this.set_folder.Name = "set_folder";
            this.set_folder.Size = new System.Drawing.Size(104, 32);
            this.set_folder.TabIndex = 3;
            this.set_folder.Text = "Select Folder";
            this.set_folder.UseVisualStyleBackColor = true;
            // 
            // registrate
            // 
            this.registrate.Location = new System.Drawing.Point(302, 216);
            this.registrate.Name = "registrate";
            this.registrate.Size = new System.Drawing.Size(112, 40);
            this.registrate.TabIndex = 4;
            this.registrate.Text = "자동 업로드 시작";
            this.registrate.UseVisualStyleBackColor = true;
            // 
            // cancleRegistrate
            // 
            this.cancleRegistrate.Location = new System.Drawing.Point(302, 278);
            this.cancleRegistrate.Name = "cancleRegistrate";
            this.cancleRegistrate.Size = new System.Drawing.Size(112, 44);
            this.cancleRegistrate.TabIndex = 5;
            this.cancleRegistrate.Text = "자동 업로드 중지";
            this.cancleRegistrate.UseVisualStyleBackColor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 12F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(491, 400);
            this.Controls.Add(this.cancleRegistrate);
            this.Controls.Add(this.registrate);
            this.Controls.Add(this.set_folder);
            this.Controls.Add(this.rudy_label);
            this.Controls.Add(this.upload_button);
            this.Controls.Add(this.scan_button);
            this.Name = "Form1";
            this.Text = "RUDY TOOL";
            this.Load += new System.EventHandler(this.Form1_Load);
            this.Click += new System.EventHandler(this.Form1_MouseClick);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button scan_button;
        private System.Windows.Forms.Button upload_button;
        private System.Windows.Forms.Label rudy_label;
        private System.Windows.Forms.Button set_folder;
        private System.Windows.Forms.Button registrate;
        private System.Windows.Forms.Button cancleRegistrate;
    }
}

