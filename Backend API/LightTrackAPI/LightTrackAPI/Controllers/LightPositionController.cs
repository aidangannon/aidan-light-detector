using System;
using System.Collections.Generic;
using System.Linq;
using System.Text.Json;
using System.Threading.Tasks;
using LightTrackAPI.Controllers;
using LightTrackAPI.Models;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.Options;
using Newtonsoft.Json.Linq;

namespace LightTrackAPI
{
    [Route("LightPosition/")]
    public class LightPositionController : ControllerBase
    {
        //data set dependancy
        private readonly ILightPositionRepos lightPositions;

        public LightPositionController(ILightPositionRepos lightPositions)
        {
            this.lightPositions = lightPositions;
        }

        [HttpPost]
        public void postLightPos([FromBody] JsonElement data)
        {
            //TODO: handle if user gives wrong key 
            //  example: "posItionX"
            //getting parameters from JSON body
            JsonElement positionX = data.GetProperty("positionX");
            JsonElement positionZ = data.GetProperty("positionZ");

            //inserts data into db and updates light positions
            lightPositions.addNewPosition(positionX, positionZ);
        }

        [HttpGet]
        public IEnumerable<LightPosition> getLightPos() {
            return lightPositions.getAllLightPosition();
        }

        [HttpGet("latest")]
        public string getLatestLightPos()
        {
            return lightPositions.getLatestPositionJson();
        }
    }
}
