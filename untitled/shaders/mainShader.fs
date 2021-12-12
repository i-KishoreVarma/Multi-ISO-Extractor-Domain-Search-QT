//#version 330
#version 410
//in vec3 FColor;
in vec3 FragPos;
in vec3 FNormal;
in vec4 FColor;

uniform int opacity;

uniform vec3 ambientColor;
uniform vec3 diffuseColor;
uniform vec3 specularColor;
uniform vec3 lightPosition;

uniform vec3 viewPosition;

void main()
{
    // ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * ambientColor;

//    vec3 objectColor = vec3(1.0, 1.0, /1.0);
    vec3 objectColor = FColor.xyz;;


    // diffuse lighting
    vec3 lightDir = normalize(lightPosition - FragPos);
    float diff = max(dot(FNormal, lightDir), 0.0);
    vec3 diffuse = 0.1 * diff * diffuseColor;


    // specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPosition - FragPos);
    vec3 reflectDir = normalize(reflect(-lightPosition, FNormal));
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);

    vec3 specular = specularStrength * spec * specularColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    // vec3 result = (specular) * objectColor;

    float cur_op = opacity;
    gl_FragColor = vec4(result.xyz, cur_op / 100.0);
}
