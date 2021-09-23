
#include "StencilPart.hpp"

StencilPart::StencilPart() 
	: Part()
{
	// description
	m_description.SetCharSize(50);
	m_description.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_description.SetString(U"Sheets Grabber [Stencil Test]");

	// sheets
	for (int i(0); i < 3; i++) {
		m_sheets[i].SetIndices({ 0,1,2, 2,3,0 });
		m_sheets[i].SetVertices({
			ae::VertexPos(-200.f, -200.f), ae::VertexPos(200.f, -200.f),
			ae::VertexPos(200.f, 200.f), ae::VertexPos(-200.f, 200.f)
		});
	}

	m_sheet_offsets[0] = ae::Vector2f(200.f, 200.f);
	m_sheet_offsets[1] = ae::Vector2f(400.f, 300.f);
	m_sheet_offsets[2] = ae::Vector2f(300.f, 400.f);
}

void StencilPart::Draw(const ae::Matrix3x3& transform) {
	m_description.Draw(transform.GetTranslated(ae::Vector2f(0.f, -50.f)));
	
	ae::StencilTest.Begin();

	// first test (prepare values)
	ae::StencilTest.SetBitModification(255);
	ae::StencilTest.SetPassCondition(ae::StencilCondition::Always, 0);
	ae::StencilTest.SetAction(ae::StencilAction::Increment, ae::StencilAction::Increment);

	for (int i(0); i < 3; i++) {
		m_sheets[i].SetColor(ae::Color::Transparent);
		m_sheets[i].Draw(transform.GetTranslated(m_sheet_offsets[i]));
	}

	// second test (drwa shapes for real)
	ae::StencilTest.SetPassCondition(ae::StencilCondition::NotEqual, 3);
	ae::StencilTest.SetAction(ae::StencilAction::Keep, ae::StencilAction::Keep);

	for (int i(0); i < 3; i++) {
		m_sheets[i].SetColor(ae::Color(30, 140, 255, 100));
		m_sheets[i].Draw(transform.GetTranslated(m_sheet_offsets[i]));
	}

	ae::StencilTest.End();
}

bool StencilPart::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	// transform mouse to world coordinates
	ae::Vector2f fixed_mouse = view.GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());

	for (int i(0); i < 3; i++) {
		ae::Vector2f left_top = model + m_sheet_offsets[i] + m_sheets[i].GetVertices()[0].position;
		ae::Vector2f right_bottom = model + m_sheet_offsets[i] + m_sheets[i].GetVertices()[2].position;

		if (
			fixed_mouse.x >= left_top.x && fixed_mouse.x < right_bottom.x &&
			fixed_mouse.y >= left_top.y && fixed_mouse.y < right_bottom.y
		) {
			m_grabbed_sheet = i;
			return true;
		}
	}

	return false;
}
void StencilPart::HandleEvent(ae::EventMouseMoved& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	// transform mouse to world coordinates
	ae::Vector2f fixed_mouse = view.GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());

	if (m_grabbed_sheet != -1)
		m_sheet_offsets[m_grabbed_sheet] = fixed_mouse - model;
}
void StencilPart::HandleEvent(ae::EventMouseButtonReleased& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {
	m_grabbed_sheet = -1;
}