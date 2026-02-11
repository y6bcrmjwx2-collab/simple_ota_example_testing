# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
idf_component_register(SRCS "LED02.c" "LED04.c" 
                    INCLUDE_DIRS "."
                    )


                    #"LED04.c" , "simple_ota_example.c"
                    #PRIV_REQUIRES esp_http_client app_update esp_https_ota
                                #  esp_netif mbedtls nvs_flash esp_wifi esp_psram
                    #EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem