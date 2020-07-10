using Dapper;
using LightTrackAPI.Models;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;
using System;
using System.Collections.Generic;
using System.Data;
using System.Data.SqlClient;
using System.Linq;
using System.Text.Json;
using System.Text.Json.Serialization;
using System.Threading.Tasks;

namespace LightTrackAPI.Controllers
{
    public class LightPositionRepos : ILightPositionRepos
    {
        List<LightPosition> LightPositions;

        private string connectionString;

        public LightPositionRepos(string connectionString) {
            this.connectionString = connectionString;
            readAllLightPosition();
        }

        private void readAllLightPosition() {
            //close connection after query
            using (IDbConnection connection = new SqlConnection(connectionString))
            {
                string procedure = "[GetAllPositions]";
                LightPositions = connection.Query<LightPosition>(procedure, commandType: CommandType.StoredProcedure).ToList();
            }
        }

        public void addNewPosition(JsonElement positionXJSON, JsonElement positionZJSON) {
            using (IDbConnection connection = new SqlConnection(connectionString))
            {
                //TODO: valid data
                //  of positionX and positionZ
                int positionX = positionXJSON.GetInt16();
                int positionZ = positionZJSON.GetInt16();

                string procedureInsert = "[AddNewPosition]";

                //creating new dynmaic class to store params
                var values = new { 
                    DateOfCapture = DateTime.Now, 
                    PositionX = positionX, 
                    PositionZ = positionZ 
                };

                //append added position to light position repos
                LightPositions.Add(
                    connection.Query<LightPosition>(
                        procedureInsert, 
                        values, 
                        commandType: CommandType.StoredProcedure
                        ).ToList().First());
            }
        }

        public IEnumerable<LightPosition> getAllLightPosition() {

            return LightPositions;
        }

        //TODO: get data dymnaically from lightPositions
        //  from memory
        //TODO: add index table in database order by Date
        //TODO: order lightPositions by Date
        public string getLatestPositionJson() {
            LightPosition recentLightPosition = LightPositions.Last();
            
            int positionX = recentLightPosition.PositionX;
            int positionZ = recentLightPosition.PositionZ;

            return JsonConvert.SerializeObject(new {positionX = positionX , positionZ = positionZ });
        }
    }
}
