namespace TextureTools
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }
        int preview_i = 0;
        CubeMap current;
        private void button1_Click(object sender, EventArgs e)
        {
            openFileDialog1.ShowDialog(this);

            var file = openFileDialog1.FileName;

            Bitmap bit = new Bitmap(file);

            int size = Math.Max(bit.Width, bit.Height);
            bit = new Bitmap(bit,new Size(size,size));

            CubeMap map = new CubeMap(bit);
            current = map;

            Preview(map.Faces[0]);
            int b = 5;

        }

        void Preview(CubeFace face)
        {

            Bitmap bm = new Bitmap(face.Final, new Size(FacePreview.Size.Width,FacePreview.Size.Height));   

            bm = new Bitmap(bm, FacePreview.Size);

            FacePreview.BackgroundImage = bm;


        }

        private void button2_Click(object sender, EventArgs e)
        {
            preview_i++;
            if (preview_i >= 6)
            {
                preview_i = 0;
            }

            Preview(current.Faces[preview_i]);
        }

        private void button3_Click(object sender, EventArgs e)
        {
            if(current == null)
            {
                MessageBox.Show("No cube map loaded.");
                return;
            }
            saveFileDialog1.ShowDialog();
            current.Export(saveFileDialog1.FileName);

        }
    }
}
