using QNet.Texture;
using System;
using System.Collections.Generic;
using System.Text;

namespace QNet.Converse
{
    public class ConvNode
    {
        
        public string Short
        {
            get;
            set;
        }

        public string Full
        {
            get;
            set;
        }

        public string Speaker
        {
            get;
            set;
        }

        public Texture2D Avatar
        {
            get;
            set;
        }

        public List<ConvNode> Nodes
        {
            get;
            set;
        }

        public ConvNode()
        {
            Nodes = new List<ConvNode>();
            Short = "";
            Full = "";
            Speaker = "";
            Avatar = null;
        }

    }
}
