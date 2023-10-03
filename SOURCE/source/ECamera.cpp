#include "pch.h"
#include "ECamera.h"
#include <SDL.h>

namespace Elite
{
	Camera::Camera(const int screenWidth, const int screenHeight, const FPoint3& position, const FVector3& viewForward, float fovAngle, float nearClip, float farClip) :
		m_Width(screenWidth),
		m_Height(screenHeight),
		m_AspectRatio{ static_cast<float>(screenWidth) / static_cast<float>(screenHeight) },
		m_Fov(tanf((fovAngle* static_cast<float>(E_TO_RADIANS)) / 2.f)),
		m_PositionLH{ position },
		m_PositionRH{ position },
		m_ViewForward{ GetNormalized(viewForward) },
		m_NearClipPlane(nearClip),
		m_FarClipPlane(farClip)
	{
		//Calculate initial matrices based on given parameters (position & target)
		CalculateLookAt();
		CalcProj();
	}

	void Camera::Update(float elapsedSec)
	{
		//Capture Input (absolute) Rotation & (relative) Movement
		//*************
		//Keyboard Input
		const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
		const float keyboardSpeed = pKeyboardState[SDL_SCANCODE_LSHIFT] ? m_KeyboardMoveSensitivity * m_KeyboardMoveMultiplier : m_KeyboardMoveSensitivity;
		m_RelativeTranslation.x = (pKeyboardState[SDL_SCANCODE_D] - pKeyboardState[SDL_SCANCODE_A]) * keyboardSpeed * elapsedSec;
		m_RelativeTranslation.y = 0;
		m_RelativeTranslation.z = (pKeyboardState[SDL_SCANCODE_S] - pKeyboardState[SDL_SCANCODE_W]) * keyboardSpeed * elapsedSec;

		//Mouse Input
		int x, y = 0;
		const uint32_t mouseState = SDL_GetRelativeMouseState(&x, &y);
		if (mouseState == SDL_BUTTON_LMASK)
		{
			m_RelativeTranslation.z += y * m_MouseMoveSensitivity * elapsedSec;
			m_AbsoluteRotation.y -= x * m_MouseRotationSensitivity;
		}
		else if (mouseState == SDL_BUTTON_RMASK)
		{
			m_AbsoluteRotation.x -= y * m_MouseRotationSensitivity;
			m_AbsoluteRotation.y -= x * m_MouseRotationSensitivity;
		}
		else if (mouseState == (SDL_BUTTON_LMASK | SDL_BUTTON_RMASK))
		{
			m_RelativeTranslation.y -= y * m_MouseMoveSensitivity * elapsedSec;
		}



		//Update LookAt (view2world & world2view matrices)
		//*************
		CalculateLookAt();
	}

	void Camera::CalculateLookAt()
	{
		{
			//FORWARD (zAxis) with YAW applied
			FMatrix3 yawRotation = MakeRotationY(m_AbsoluteRotation.y * static_cast<float>(E_TO_RADIANS));
			FVector3 zAxis = yawRotation * -m_ViewForward;

			//Calculate RIGHT (xAxis) based on transformed FORWARD
			const FVector3 xAxis = GetNormalized(Cross(FVector3{ 0.f,1.f,0.f }, zAxis));

			//FORWARD with PITCH applied (based on xAxis)
			FMatrix3 pitchRotation = MakeRotation(m_AbsoluteRotation.x * static_cast<float>(E_TO_RADIANS), xAxis);
			zAxis = pitchRotation * zAxis;

			//Calculate UP (yAxis)
			const FVector3 yAxis = Cross(zAxis, xAxis);

			//Translate based on transformed axis
			m_PositionLH += m_RelativeTranslation.x * xAxis;
			m_PositionLH += m_RelativeTranslation.y * yAxis;
			m_PositionLH += -m_RelativeTranslation.z * zAxis;

			//Construct View2World Matrix
			m_ViewToWorldLH =
			{
				FVector4{xAxis},
				FVector4{yAxis},
				FVector4{zAxis},
				FVector4{m_PositionLH.x,m_PositionLH.y,m_PositionLH.z,1.f}
			};

			//Construct World2View Matrix
			m_WorldToViewLH = Inverse(m_ViewToWorldLH);
		}

		
		{
			//FORWARD (zAxis) with YAW applied
			FMatrix3 yawRotation = MakeRotationY(m_AbsoluteRotation.y * float(E_TO_RADIANS));
			FVector3 zAxis = yawRotation * m_ViewForward;

			//Calculate RIGHT (xAxis) based on transformed FORWARD
			FVector3 xAxis = GetNormalized(Cross(FVector3{ 0.f,1.f,0.f }, zAxis));

			//FORWARD with PITCH applied (based on xAxis)
			FMatrix3 pitchRotation = MakeRotation(m_AbsoluteRotation.x * float(E_TO_RADIANS), xAxis);
			zAxis = pitchRotation * zAxis;

			//Calculate UP (yAxis)
			FVector3 yAxis = Cross(zAxis, xAxis);

			//Translate based on transformed axis
			m_PositionRH += m_RelativeTranslation.x * xAxis;
			m_PositionRH += m_RelativeTranslation.y * yAxis;
			m_PositionRH += m_RelativeTranslation.z * zAxis;

			//Construct View2World Matrix
			m_ViewToWorldRH =
			{
				FVector4{xAxis},
				FVector4{yAxis},
				FVector4{zAxis},
				FVector4{m_PositionRH.x,m_PositionRH.y,m_PositionRH.z,1.f}
			};

			//Construct World2View Matrix
			m_WorldToViewRH = Inverse(m_ViewToWorldRH);
		}
	}

	void Camera::CalcProj()
	{
			m_ProjectionLH = FMatrix4::Identity();
			m_ProjectionLH.data[0][0] = 1.f / (GetAspectRatio() * GetFov());
			m_ProjectionLH.data[1][1] = 1.f / GetFov();
			m_ProjectionLH.data[2][2] = m_FarClipPlane / (m_FarClipPlane - m_NearClipPlane);
			m_ProjectionLH.data[2][3] = 1.f;
			m_ProjectionLH.data[3][2] = -(m_FarClipPlane * m_NearClipPlane) / (m_FarClipPlane - m_NearClipPlane);
			m_ProjectionLH.data[3][3] = 0.f;

			m_ProjectionRH = FMatrix4::Identity();
			m_ProjectionRH.data[0][0] = 1.f / (GetAspectRatio() * GetFov());
			m_ProjectionRH.data[1][1] = 1.f / GetFov();
			m_ProjectionRH.data[2][2] = m_FarClipPlane / (m_NearClipPlane - m_FarClipPlane);
			m_ProjectionRH.data[2][3] = -1.f;
			m_ProjectionRH.data[3][2] = (m_FarClipPlane * m_NearClipPlane) / (m_NearClipPlane - m_FarClipPlane);
			m_ProjectionRH.data[3][3] = 0.f;
	}
}
 