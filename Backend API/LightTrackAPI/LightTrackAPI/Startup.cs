using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Configuration;
using LightTrackAPI.Models;
using LightTrackAPI.Controllers;
using System.Data;
using System.Data.SqlClient;

namespace LightTrackAPI
{
    public class Startup
    {

        private IConfiguration configuration;

        public Startup(IConfiguration configuration) {
            this.configuration = configuration;
        }

        public void ConfigureServices(IServiceCollection services)
        {
            //inject LightPositionRepository into our code
            //to be accessed by our controllers via contstructor

            //getting connection string via appsettings.json
            services.AddSingleton<ILightPositionRepos>(
                s => new LightPositionRepos(
                    configuration.GetSection("ConnectionStrings").GetSection("LightTracker").Value));
            services.AddControllers();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env)
        {
            if (env.IsDevelopment())
            {
                app.UseDeveloperExceptionPage();
            }

            app.UseRouting();

            app.UseEndpoints(endpoints =>
            {
                endpoints.MapControllers();
            });
        }
    }
}
