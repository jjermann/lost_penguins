#ifndef DEF_ANIMATION_H
#define DEF_ANIMATION_H 1

/** \brief Animation is responsible to return the correct frame of an animation

*/
class Animation {
    public:
        /// \brief Initialize the animation
        /// \param abase_image Base image used for the animation
        /// \param aduration Duration of the animation
        /// \param aframes Number of frames
        /// \param aanimation_type Animation type
        /// \param astart_pos Start position from the frame array of the base_image
        /// \param abp_type Base point type
        /// \param aallign_type Allignment type
        /// \param ashift_x x shift value
        /// \param ashift_y y shift value
        ///
        /// To load one still image simply use: Animation(loadImage(1,"image_name.png"))
        Animation(const Image& abase_image,
                  Uint32 aduration=0,
                  Uint16 aframes=1,
                  Uint16 aanimation_type=ATYPE_LOOP,
                  Uint16 astart_pos=0,
                  BasePointType abp_type=BP_MD,
                  AllignType aallign_type=AT_MD,
                  Sint16 ashift_x=0,
                  Sint16 ashift_y=0);
        ~Animation();
        /// Set the base position of the animation
        void setBasePos(SDL_Rect* abase_pos) {
            base_pos=abase_pos;
            checkAnim();
        }
        /// Unsets the base position of the animation
        void unsetBasePos() {
            base_pos=NULL;
        }
        /// Start the animation from the beginning (used when the animation was still)
        void runAnim();
        /// Updates a running animation and stops it if necessary
        /// return True if the animation is still running
        bool updateAnim(Uint16 dt);
        /// Calculates and returns the current frame
        Frame getFrame() const;
        /// Calculates and returns the current draw position using:
        /// base_pos,bp_type,allign_type,shift,cur_im_pos
        SDL_Rect getDrawPos() const;
        /// HACK: Return the dimensions of the first frame
        SDL_Rect getFrameDim() const;
        /// HACK: Return the base frame
        Frame getBaseFrame() const;
        /// return True if the animation is valid
        bool isValid() const {
            return is_valid;
        }
        /// return True if the animation is running
        bool isRunning() const {
            return is_running;
        }   
        /// return True if the animation is a still image
        bool isImage() const {
            return is_image;
        }   
        /// return True if the animation dimensions stay the same
        bool isFixed() const {
            return is_fixed;
        }   
    private:
        /// Helper function to stop an animation
        bool stopAnim();
        /// check validity of the animation
        bool checkAnim();
    private:
        /// Base image for the animation (big)
        Image base_image;
        /// Duration of the animation in ms
        Uint32 duration;
        /// Number of frames
        Uint16 frames;
        /// Animation type: ATYPE_ONCE (play once), ATYPE_ONCE_END (play once and stop at end),
        /// ATYPE_LOOP (always play, jump back to the beginning), ATYPE_SWING (always play,
        /// reverse direction when finnished). The appended _REV means that the animation is
        /// started at the end and played backwards.
        Uint16 animation_type;
        /// Start position from the frame array of the base_image
        Uint16 start_pos;
        ///\brief The base point of the animation
        ///
        /// The base point always stays fixed during the animation.
        /// It's types are: BP_xy, where x may be either L,M or R and y may be either
        /// U, M or D. L: left end, M: middle, R: right end, U: upper end, D: lower end
        ///
        /// Example: BP_MU means the BP is the upper middle point of each frame.
        /// Default: BP_MD
        BasePointType bp_type;
        ///\brief Allignment of the base point onto the base_pos
        ///
        /// The allignment specifies how the base point should be positioned wrt to the
        /// base_pos: It's type are basically the same as in BasePointType.
        ///
        /// Example: (Assuming BP_MD) AT_LD means the middle point of each frame (BP_MD)
        ///          is mapped onto the lower left edge of the base_pos
        /// Default: AT_MD
        AllignType allign_type;
        //@{
        /// shift is added as an additional value to the designated BP position.
        Sint16 shift_x;
        Sint16 shift_y;
        //@}
        
        /* calculated or given values/functions */
        /// End position from the frame array of the base_image
        Uint16 end_pos;
        /// True if the animation consist of one still image (frames=1,still=true)
        bool is_image;
        /// True if the sanity checks suceeded
        bool is_valid;
        /// True if the image dimensions stay fixed during the whole animation (unused)
        bool is_fixed;

        /* temporary values */
        /// Pointer to the current base position of the animation
        SDL_Rect* base_pos;
        /// True if the animation is running
        bool is_running;
        /// True if the animation is running forward at the moment
        bool forward;
        /// Current frame number
        Uint16 cur_frame_num;
        /// Current position of the base_image_desc array
        Uint16 cur_im_pos;
        /// Current time
        Uint32 cur_time;
};

#endif
