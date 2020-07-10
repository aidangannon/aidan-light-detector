using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace LightTrackAPI.Models
{
    public class LightPosition
    {
        public int Id { get; }
        public DateTime DateTimeOfCapture { get; set; }

        public int PositionX { get; set; }

        public int PositionZ { get; set; }
    }
}
