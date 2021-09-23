
#include "VertexGrabberPart.hpp"

constexpr int c_grab_vertices_count = 12;
constexpr float c_pi = 3.14159265f, c_grab_radius = 200.f, c_grab_point_size = 20.f;

constexpr float c_box_padding = 30.f, c_box_offset = 10.f;

VertexGrabberInterface::VertexGrabberInterface(const std::function<void(ae::DrawMode)>& draw_mode_callback)
	: LocalInterface(), m_draw_mode_callback(draw_mode_callback)
{
	m_title.SetCharSize(20);
	m_title.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_title.SetString(U"== Vertex Grabber ==");

	m_draw_mode_boxes[0].SetFamily("vertex_grabber");
	m_draw_mode_boxes[0].SetShape(GUIChooseBox::Shape::Circle);
	m_draw_mode_boxes[0].SetString(U"Field [TriangleFan]");
	m_draw_mode_boxes[0].AllowManualUncheck(false);

	m_draw_mode_boxes[1].SetFamily("vertex_grabber");
	m_draw_mode_boxes[1].SetShape(GUIChooseBox::Shape::Circle);
	m_draw_mode_boxes[1].SetString(U"Length [LineLoop]");
	m_draw_mode_boxes[1].AllowManualUncheck(false);

	m_draw_mode_boxes[2].SetFamily("vertex_grabber");
	m_draw_mode_boxes[2].SetShape(GUIChooseBox::Shape::Circle);
	m_draw_mode_boxes[2].SetString(U"Count [Points]");
	m_draw_mode_boxes[2].AllowManualUncheck(false);

	m_draw_mode_boxes[0].SetChecked(true);
}

void VertexGrabberInterface::Draw(const ae::Matrix3x3& transform) {
	m_title.Draw(transform);

	for (int i(0); i < 3; i++)
		m_draw_mode_boxes[i].Draw(transform.GetTranslated(ae::Vector2f(0.f, c_box_offset + i * c_box_padding)));
}

bool VertexGrabberInterface::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) {
	for (int i(0); i < 3; i++)
		if (m_draw_mode_boxes[i].HandleEvent(event, ae::Vector2f(position.x, position.y + c_box_offset + i * c_box_padding))) {
			
			if (m_draw_mode_callback) {
				ae::DrawMode draw_mode =
					i == 0 ? ae::DrawMode::TriangleFan :
					i == 1 ? ae::DrawMode::LineLoop :
					ae::DrawMode::Points
				;
				m_draw_mode_callback(draw_mode);
			}
			return true;
		}
	
	return false;
}
void VertexGrabberInterface::HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) {
	for (int i(0); i < 3; i++)
		m_draw_mode_boxes[i].HandleEvent(event, ae::Vector2f(position.x, position.y + c_box_offset + i * c_box_padding));
}

VertexGrabberPart::VertexGrabberPart() 
	: Part(ae::FloatRect(-200.f, -200.f, 1000.f, 1000.f), "vertex_grabber")
{
	// points
	m_grab_point_vertices.Bind();
	m_grab_point_vertices.AddLayout<ae::Vector2f>(0, offsetof(ae::VertexPos, ae::VertexPos::position), false);

	constexpr float grab_offset = c_grab_point_size / 2.f;
	m_grab_point_vertices.SetVertices({
		ae::VertexPos(-grab_offset, -grab_offset), ae::VertexPos(grab_offset, -grab_offset),
		ae::VertexPos(grab_offset, grab_offset), ae::VertexPos(-grab_offset, grab_offset)
	});
	m_grab_point_vertices.SetIndices({ 0,1,2, 2,3,0 });

	m_grab_vertices.Bind();
	m_grab_vertices.AddLayout<ae::Vector2f>(0, offsetof(ae::VertexPos, ae::VertexPos::position), false);
	m_grab_vertices.SetDrawMode(ae::DrawMode::TriangleFan);

	// shape
	std::vector<ae::VertexPos> grab_vertices;
	std::vector<unsigned int> grab_indices;

	constexpr int grab_vertices_count = 12;
	constexpr float pi = 3.14159265f, grab_radius = 200.f;

	for (int v(0); v < c_grab_vertices_count; v++) {
		float angle = v * (2.f * c_pi) / c_grab_vertices_count;

		grab_vertices.emplace_back(
			(cos(angle) + 1.f) * c_grab_radius,
			(sin(angle) + 1.f) * c_grab_radius
		);
		grab_indices.push_back(v);

		m_grab_point_offsets.push_back(grab_vertices.back().position);
	}
	m_grab_vertices.SetVertices(std::move(grab_vertices));
	m_grab_vertices.SetIndices(std::move(grab_indices));

	// error message
	m_error_text.SetCharSize(50);
	m_error_text.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_error_text.SetColor(ae::Color(255, 100, 100, 255));
	m_error_text.SetString(U"Error caught!\nvertex_grabber.vsh / vertex_grabber.fsh\nfile not found!");

	// description
	m_description.SetCharSize(50);
	m_description.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_description.SetString(U"Here, grab a vertex!");

	m_subdescription.SetCharSize(35);
	m_subdescription.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_subdescription.SetString(U"(move closer to see an options menu)");

	// local interface
	LocalInterface::Create<VertexGrabberInterface>(
		"vertex_grabber", 
		[this](ae::DrawMode draw_mode) {
			m_grab_vertices.SetDrawMode(draw_mode);
		}
	);
}
VertexGrabberPart::~VertexGrabberPart() {
	LocalInterface::Delete("vertex_grabber");
}

void VertexGrabberPart::Draw(const ae::Matrix3x3& transform) {

	ae::Shader& color_shader = ae::DefaultAssets->color_shader;
	ae::Shader& points_shader = ae::AssetManager.GetShader("vertex_grabber");

	// error
	if (points_shader == ae::AssetManager.GetDefaultShader()) {
		const ae::FloatRect& bounds = m_error_text.GetBounds();
		m_error_text.Draw(transform.GetTranslated(ae::Vector2f(0.f, bounds.top + bounds.height)));
		return;
	}

	// shape
	color_shader.Bind();
	color_shader.SetUniform.Vec4f("u_color", ae::Color(30, 140, 255, 100));
	color_shader.SetUniform.Mat3x3("u_mvp", transform.GetArray());

	m_grab_vertices.Bind();
	m_grab_vertices.Draw();

	// points
	points_shader.Bind();
	points_shader.SetUniform.Vec4f("u_color", ae::Color(20, 100, 190, 200));
	points_shader.SetUniform.Mat3x3("u_vp", transform.GetArray());
	points_shader.SetUniform.Vec2fArray("u_positions", m_grab_point_offsets.data(), 0, c_grab_vertices_count);

	m_grab_point_vertices.Bind();
	m_grab_point_vertices.DrawInstanced(c_grab_vertices_count);

	// description
	m_description.Draw(transform.GetTranslated(ae::Vector2f(0.f, -90.f)));
	m_subdescription.Draw(transform.GetTranslated(ae::Vector2f(-60.f, -50.f)));
}

bool VertexGrabberPart::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	// transform mouse to world coordinates
	ae::Vector2f fixed_mouse = view.GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());

	for (size_t i(0); i < c_grab_vertices_count; i++)
	{
		const ae::Vector2f& local_left_top = m_grab_point_vertices.GetVertex(0).position;
		const ae::Vector2f& local_right_bottom = m_grab_point_vertices.GetVertex(2).position;

		ae::Vector2f left_top = model + m_grab_point_offsets[i] + local_left_top;
		ae::Vector2f right_bottom = model + m_grab_point_offsets[i] + local_right_bottom;

		if (
			fixed_mouse.x >= left_top.x && fixed_mouse.x < right_bottom.x &&
			fixed_mouse.y >= left_top.y && fixed_mouse.y < right_bottom.y
		) {
			m_grabbed_vertex = i;
			return true;
		}
	}

	return false;
}
void VertexGrabberPart::HandleEvent(ae::EventMouseMoved& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	if (m_grabbed_vertex != -1) {
		ae::Vector2f pos = view.GetTranslated(model).GetInverse().TransformPoint(event.GetPosition());
		m_grab_point_offsets[m_grabbed_vertex] = pos;
		m_grab_vertices.SetVertex(m_grabbed_vertex, pos);
	}
}

void VertexGrabberPart::HandleEvent(ae::EventMouseButtonReleased& event, const ae::Matrix3x3& view_model, const ae::Vector2f& model) {
	m_grabbed_vertex = -1;
}