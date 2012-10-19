class Camera
{
        float yaw, pitch;
    public:
        float x,y,z;
        void strafe(float);
        void move(float);
        void updateYaw(float);
        void updatePitch(float);
        float getYaw();
        float getPitch();
};
