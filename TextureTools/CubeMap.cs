using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace TextureTools
{
    public class CubeFace
    {
        public int W, H;
        public byte[] Data;
        public Bitmap Final;
    }
    public class CubeMap
    {
        Bitmap map;
        public List<CubeFace> Faces;
        public CubeMap(Bitmap cm)
        {

            map = cm;
            Faces = new List<CubeFace>();
            Faces.Add(GetFace(0.251f, 0));
            Faces.Add(GetFace(0, 0.333333f));
            Faces.Add(GetFace(0.25f, 0.33333f));
            Faces.Add(GetFace(0.5f, 0.33333f));
            Faces.Add(GetFace(0.75f, 0.33333f));
            Faces.Add(GetFace(0.25f, 0.666666f));
            CorrectFaces();

        }

        void CorrectFaces()
        {

            int size = Math.Max(Faces[0].W, Faces[0].H);

            byte[] nd = new byte[size * size * 3];

            foreach (var face in Faces)
            {

                float xi =  (float)(size) / (float)(face.W);
                float yi =  (float)size / (float)(face.H);


                Bitmap bm = new Bitmap(size, size);
                face.Final = bm;
                for (int y = 0; y < size; y++)
                {
                    for (int x = 0; x < size; x++)
                    {
                        int r, g, b;

                        int sx = (int)(((float)x / xi));
                        int sy = (int)(((float)y / yi));

                        int loc = (sy * face.W * 3) + (sx * 3);

                        r = face.Data[loc++];
                        g = face.Data[loc++];
                        b = face.Data[loc++];

                        Color c = Color.FromArgb(r, g, b);

                        bm.SetPixel(x, y, c);
                        
                    }
                }

                face.W = size;
                face.H = size;
            }


        }

        public CubeFace GetFace(float sx,float sy)
        {
            int px, py;

            px = (int)((float)map.Width * sx);
            py = (int)((float)map.Height * sy);
            CubeFace face = new CubeFace();
            face.W = (int)((float)map.Width * 0.25f);
            face.H = (int)((float)map.Height*0.33f);

            face.Data = new byte[face.W * face.H * 3];

            for(int y = 0; y < face.H; y++)
            {
                for(int x = 0; x < face.W; x++)
                {
                    
                    int rx = px + x;
                    int ry = py + y;

                    var pix = map.GetPixel(rx, ry);
                    int npix = (y * face.W * 3) + (x * 3);
                    face.Data[npix] = pix.R;
                    face.Data[npix + 1] = pix.G;
                    face.Data[npix + 2] = pix.B;


                }
            }
            return face;

        }

        void WriteFace(BinaryWriter w,CubeFace face)
        {
            

                for (int y = 0; y < face.H; y++)
                {
                    for (int x = 0; x < face.W; x++)
                    {
                        Color pix = face.Final.GetPixel(x, y);
                        w.Write((byte)pix.R);
                        w.Write((byte)pix.G);
                        w.Write((byte)pix.B);
                    }

                }
      
        }
        void WriteFaceBlank(BinaryWriter w)
        {

            var face = Faces[0];

            for (int y = 0; y < face.H; y++)
            {
                for (int x = 0; x < face.W; x++)
                {
                    Color pix = face.Final.GetPixel(x, y);
                    pix = Color.FromArgb(0, 0, 0);
                    w.Write((byte)pix.R);
                    w.Write((byte)pix.G);
                    w.Write((byte)pix.B);
                }

            }

        }
        public void Export(string file)
        {

            FileStream fs = new(file, FileMode.OpenOrCreate,FileAccess.Write);
            BinaryWriter w = new(fs);

            

            w.Write(Faces[0].W);
            w.Write(Faces[1].H);

            int xp = 3;
            int xm = 1;
            int yp = 0;
            int ym = 5;
            int zp = 4;

            //0
            WriteFace(w, Faces[3]); //XPlus
            //WriteFaceBlank(w);

            //1
            WriteFace(w, Faces[1]); //XMin
            //WriteFaceBlank(w);

            //2
            WriteFace(w, Faces[0]);

            //WriteFaceBlank(w);

            //3
            //WriteFaceBlank(w);
            WriteFace(w, Faces[5]);

            //4 
            WriteFace(w, Faces[2]);

            //WriteFaceBlank(w);
            WriteFace(w, Faces[4]);


            /*
             * 
             * 
                    //0
            WriteFace(w, Faces[3]); //XPlus
            //WriteFaceBlank(w);

            //1
            WriteFace(w, Faces[1]); //XMin
            //WriteFaceBlank(w);

            //2
            WriteFace(w, Faces[0]);

            //WriteFaceBlank(w);

            //3
            //WriteFaceBlank(w);
            WriteFace(w, Faces[5]);

            //4 
            WriteFace(w, Faces[4]);

            //WriteFaceBlank(w);
            WriteFace(w, Faces[2]);
             */

            //WriteFaceBlank(w);
            // WriteFace(w, Faces[2]);
            //WriteFace(w, Faces[1]);
            // WriteFace(w, Faces[0]);
            // WriteFace(w, Faces[5]);
            // WriteFace(w, Faces[3]);
            //WriteFace(w, Faces[4]);


            //            WriteFace(w, Faces[4]);
            //          WriteFace(w, Faces[1]);
            //         WriteFace(w, Faces[0]);
            //        WriteFace(w, Faces[5]);
            //       WriteFace(w, Faces[4]);
            //      WriteFace(w, Faces[3]);

            w.Flush();
            fs.Flush();
            fs.Close();

        }

    }
}
