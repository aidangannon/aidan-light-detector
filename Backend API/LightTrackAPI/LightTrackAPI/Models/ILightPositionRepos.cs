using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;

namespace LightTrackAPI.Models
{
    public interface ILightPositionRepos
    {
        public IEnumerable<LightPosition> getAllLightPosition();
        public void addNewPosition(JsonElement positionXJSON, JsonElement positionZJSON);
        public string getLatestPositionJson();
    }
}
