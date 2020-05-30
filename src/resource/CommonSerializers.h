#pragma once
class CommonSerializers {
public:
    static void CommonParsers();
};

/* test scene

Material uved_mat(vec4(1), vec4(.5), 25);
uved_mat.diffuseMap = R->getTexture("uvmap");
uved_mat.shader = Program(uved_v, uved_f);

Entity* monkey = new Entity(R->getGeometry("monkey"), uved_mat);
monkey->setName("monkey");

Entity* monkey2 = new Entity(R->getGeometry("monkey"), uved_mat);
monkey2->setName("monkey2");
monkey2->transform.translate(vec3(2, 2, 0));

Entity* ground = new Entity(R->getGeometry("ground"), uved_mat);
ground->transform.translate(vec3(0, -1, 0));
ground->transform.scale(vec3(10));

Entity* ball = new Entity(R->getGeometry("knot"), uved_mat);

Node* scene = level1.GetRoot();

Camera* mainCamera = new Camera();
mainCamera->setName("camera 1");
mainCamera->transform.translate(vec3(0, 3, -5));
mainCamera->transform.rotate(glm::radians(30.f), glm::vec3(1, 0, 0));

Camera* camera2 = new Camera();
camera2->setName("camera 2");
camera2->transform.translate(vec3(0, 0, 6));
camera2->transform.rotate(glm::radians(180.f), glm::vec3(0, 1, 0));

Camera* camera3 = new Camera();
camera3->setName("camera 3");
camera3->UpdateProjection(height, width, CameraSettings{
		.mode = CameraSettings::Mode::Orthographic,
		.OrthographicData = {
			.NearPlane = 1.,
			.FarPlane = 100.,
			.Left = -4.,
			.Right = 4.,
			.Bottom = -4.,
			.Top = 4.
		}
	});
camera3->transform.translate(vec3(0, 5, 5));
camera3->transform.rotate(glm::radians(180.), glm::vec3(0, 1, 0));
camera3->transform.rotateBy(glm::angleAxis(glm::radians(-45.f), glm::vec3(1, 0, 0)));

player = new Player(R->getGeometry("monkey"), uved_mat);

scene->add(player);
player->add(mainCamera);

scene->add(camera3);
camera3->add(ball);

scene->add(ground);

scene->add(monkey);
monkey->add(monkey2);
monkey2->add(camera2);

level1.SetMainCamera("camera 1");

PointLight* whiteLight = new PointLight(Light::PointData{
	vec4(1),vec4(1),vec4(1),
	Light::attunation{1. / 3.,1,1},
	0,
	vec4(0)
});
whiteLight->transform.translate(vec3(0, 0, 3));

PointLight* redLight = new PointLight(Light::PointData{
	vec4(.5),vec4(1.,0.,0.,1.),vec4(1),
	Light::attunation{1. / 3.,1,1},
	0,
	vec4(0)
});
redLight->transform.translate(vec3(3, 0, 0));

scene->add(whiteLight);
monkey2->add(redLight);*/